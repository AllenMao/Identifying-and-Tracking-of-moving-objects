#include "MeanShift.h"

IplImage *image = 0, *hsv = 0, *hue = 0, *mask = 0, *backproject = 0, *histimg = 0;//用HSV中的Hue分量进行跟踪
CvHistogram *hist = 0;//直方图类

int backproject_mode = 0;//表示是否要进入反向投影模式，ture表示准备进入反向投影模式
int select_object = 0;//代表是否在选要跟踪的初始目标，true表示正在用鼠标选择
int track_object = 0;//代表跟踪目标数目
int show_hist = 1;//是否显示直方图
CvPoint origin;//用于保存鼠标选择第一次单击时点的位置
CvRect selection;//用于保存鼠标选择的矩形框
CvRect track_window;
CvBox2D track_box; // Meanshift跟踪算法返回的Box类
CvConnectedComp track_comp;

int hdims = 50; // 划分直方图bins的个数，越多越精确
float hranges_arr[] = {0,180};//像素值的范围
float* hranges = hranges_arr;//用于初始化CvHistogram类
int NumFrames=0;

CvScalar hsv2rgb( float hue )//用于将Hue量转换成RGB量
{
	int rgb[3], p, sector;
	static const int sector_data[][3]={{0,2,1}, {1,2,0}, {1,0,2}, {2,0,1}, {2,1,0}, {0,1,2}};
	hue *= 0.033333333333333333333333333333333f;
	sector = cvFloor(hue);
	p = cvRound(255*(hue - sector));
	p ^= sector & 1 ? 255 : 0;

	rgb[sector_data[sector][0]] = 255;
	rgb[sector_data[sector][1]] = 0;
	rgb[sector_data[sector][2]] = p;

	return cvScalar(rgb[2], rgb[1], rgb[0],0);//返回对应的颜色值
}

int MeanShift(CvRect selection,IplImage* frame)
{
	int vmin = 10, vmax = 256, smin = 30;
	NumFrames++;
	cvNamedWindow( "CamShiftDemo", 1 );//建立视频窗口

	cvCreateTrackbar( "Vmin", "CamShiftDemo", &vmin, 256, 0 );//建立滑动条
	cvCreateTrackbar( "Vmax", "CamShiftDemo", &vmax, 256, 0 );
	cvCreateTrackbar( "Smin", "CamShiftDemo", &smin, 256, 0 );

	int i, bin_w, c;
	if( !frame )
		return -1;
	else track_object=-1;

	if(NumFrames==1)//刚开始先建立一些缓冲区
	{ 
		image = cvCreateImage( cvGetSize(frame), 8, 3 );//
		image->origin = frame->origin;//origin为0表示顶左结构，即图像的原点是左上角，如果为1为左下角

		hsv = cvCreateImage( cvGetSize(frame), 8, 3 );
		hue = cvCreateImage( cvGetSize(frame), 8, 1 );

		mask = cvCreateImage( cvGetSize(frame), 8, 1 );//分配掩膜图像空间

		backproject = cvCreateImage( cvGetSize(frame), 8, 1 );//分配反向投影图空间，大小一样，单通道

		hist = cvCreateHist( 1, &hdims, CV_HIST_ARRAY, &hranges, 1 ); //分配建立直方图空间

		histimg = cvCreateImage( cvSize(320,200), 8, 3);//分配用于画直方图的空间
		cvZero( histimg );//背景为黑色
	}

	cvCopy( frame, image, 0 );
	cvCvtColor( image, hsv, CV_BGR2HSV ); // 把图像从RGB表色系转为HSV表色系

	if(track_object)// 如果当前有需要跟踪的物体
	{
		int _vmin = vmin, _vmax = vmax;
		//inRange函数的功能是检查输入数组每个元素大小是否在2个给定数值之间，可以有多通道,mask保存0通道的最小值，也就是h分量，左边三个最小到右边三个最大
		cvInRangeS( hsv, cvScalar(0,smin,MIN(_vmin,_vmax),0),cvScalar(180,256,MAX(_vmin,_vmax),0), mask ); //制作掩膜板，只处理像素值为H：0~180，S：smin~256，V：vmin~vmax之间的部分
		cvSplit( hsv, hue, 0, 0, 0 ); // 取得H分量

		if( track_object < 0 )//如果需要跟踪的物体还没有进行属性提取，则进行选取框类的图像属性提取
		{
			float max_val = 0.f;
			cvSetImageROI( hue, selection ); // 设置原选择框
			cvSetImageROI( mask, selection ); // 设置Mask的选择框
			//calcHist()函数第一个参数为输入矩阵序列，第2个参数表示输入的矩阵数目，第3个参数表示将被计算直方图维数通道的列表，第4个参数表示可选的掩码函数
			//第5个参数表示输出直方图，第6个参数表示直方图的维数，第7个参数为每一维直方图数组的大小，第8个参数为每一维直方图bin的边界
			cvCalcHist( &hue, hist, 0, mask ); // 得到选择框内且满足掩膜板内的直方图

			cvGetMinMaxHistValue( hist, 0, &max_val, 0, 0 ); 
			cvConvertScale( hist->bins, hist->bins, max_val ? 255. / max_val : 0., 0 ); // 对直方图转为0~255

			cvResetImageROI( hue ); // remove ROI
			cvResetImageROI( mask );
			track_window = selection;
			track_object = 1;//只要鼠标选完区域松开后，且没有按键盘清0键'c'，则trackObject一直保持为1，因此该if函数只能执行一次，除非重新选择跟踪区域

			cvZero( histimg );
			//histing是一个200*300的矩阵，hsize应该是每一个bin的宽度，也就是histing矩阵能分出几个bin出来
			bin_w = histimg->width / hdims;
			for( i = 0; i < hdims; i++ )
			{
				int val = cvRound( cvGetReal1D(hist->bins,i)*histimg->height/255 );
				CvScalar color = hsv2rgb(i*180.f/hdims);

				cvRectangle( histimg, cvPoint(i*bin_w,histimg->height),
					cvPoint((i+1)*bin_w,histimg->height - val),color, -1, 8, 0 );//画直方图到图像空间
			}
		}

		cvCalcBackProject( &hue, backproject, hist ); // 得到hue的反向投影图
		cvNamedWindow("df",1);
		cvShowImage("df",backproject);


		cvAnd( backproject, mask, backproject, 0 );//得到反向投影图mask内的内容
		cvCamShift( backproject, track_window,cvTermCriteria( CV_TERMCRIT_EPS | CV_TERMCRIT_ITER, 10, 1 ),&track_comp, &track_box );//使用MeanShift算法对backproject中的内容进行搜索，返回跟踪结果
		track_window = track_comp.rect;//得到跟踪结果的矩形框

		if( backproject_mode )
			cvCvtColor( backproject, image, CV_GRAY2BGR ); // 显示模式
		if( image->origin )
			track_box.angle = -track_box.angle;
		cvEllipseBox( image, track_box, CV_RGB(255,0,0), 3, CV_AA, 0 );//画出跟踪结果的位置
	}

	if( select_object && selection.width > 0 && selection.height > 0 )//如果正处于物体选择，画出选择框
	{
		cvSetImageROI( image, selection );
		cvXorS( image, cvScalarAll(255), image, 0 );
		cvResetImageROI( image );
	}
	

	cvShowImage( "CamShiftDemo", image );//显示视频和直方图
	cvShowImage( "Histogram", histimg );
	cvWaitKey(1000);
}

