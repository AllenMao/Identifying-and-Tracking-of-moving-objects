/**************************************************
* 背景建模，运动物体检测
*      
**************************************************/
#pragma once

#include"Main.h"

int count1=0;

FrameDifference::FrameDifference(bool bReadFromFile, char *fileName, char *fileName1)
{
	/*meanshift-begin*/
	image = 0, hsv = 0, hue = 0, mask = 0, backproject = 0, histimg = 0;//用HSV中的Hue分量进行跟踪
	hist = 0;//直方图类

	backproject_mode = 0;//表示是否要进入反向投影模式，ture表示准备进入反向投影模式
	select_object = 0;//代表是否在选要跟踪的初始目标，true表示正在用鼠标选择
	track_object = 0;//代表跟踪目标数目
	show_hist = 1;//是否显示直方图
	origin;//用于保存鼠标选择第一次单击时点的位置
	selection;//用于保存鼠标选择的矩形框
	track_window;
	track_box; // Meanshift跟踪算法返回的Box类
	track_comp;

	hdims = 50; // 划分直方图bins的个数，越多越精确

	NumFrames=0;
	vmin = 10, vmax = 256, smin = 30;
	/*meanshift-end*/
	/*meanshift1-begin*/
	image1 = 0, hsv1 = 0, hue1 = 0, mask1 = 0, backproject1 = 0, histimg1 = 0;//用HSV中的Hue分量进行跟踪
	hist1 = 0;//直方图类

	backproject_mode1 = 0;//表示是否要进入反向投影模式，ture表示准备进入反向投影模式
	select_object1 = 0;//代表是否在选要跟踪的初始目标，true表示正在用鼠标选择
	track_object1 = 0;//代表跟踪目标数目
	show_hist1 = 1;//是否显示直方图
	origin1;//用于保存鼠标选择第一次单击时点的位置
	selection1;//用于保存鼠标选择的矩形框
	track_window1;
	track_box1; // Meanshift跟踪算法返回的Box类
	track_comp1;

	hdims1 = 50; // 划分直方图bins的个数，越多越精确

	NumFrames1=0;
	vmin1 = 10, vmax1 = 256, smin1 = 30;
	/*meanshift1-end*/

	//声明IplImage指针
	IplImage* pFrame = NULL;


	IplImage* pFrameImg = NULL; 
	IplImage* pFrImg = NULL;
	IplImage* pBkImg = NULL;
	IplImage* pEndImg=NULL;

	


	CvMat* pFrMat = NULL;
	CvMat* pEndMat = NULL;
	CvMat* pBkMat = NULL;

	CvCapture* pCapture = NULL;


	int nFrmNum = 0;

	//声明IplImage指针1
	IplImage* pFrame1 = NULL;

	IplImage* pFrameImg1 = NULL; 
	IplImage* pFrImg1 = NULL;
	IplImage* pBkImg1 = NULL;
	IplImage* pEndImg1=NULL;

	


	CvMat* pFrMat1 = NULL;
	CvMat* pEndMat1 = NULL;
	CvMat* pBkMat1 = NULL;


	CvCapture* pCapture1 = NULL;

	int nFrmNum1 = 0;
	/////////////////////////////////////////
	//创建窗口
	//cvNamedWindow("video", 1);
	//cvNamedWindow("background",1);
	//cvNamedWindow("foreground",1);
	/*meanshift*/
	cvNamedWindow( "CamShiftDemo", 1 );//建立视频窗口
	cvNamedWindow( "CamShiftDemo1", 1 );//建立视频窗口
	cvMoveWindow("CamShiftDemo", 30, 0);
	cvMoveWindow("CamShiftDemo1", 360, 0);


	//使窗口有序排列
	//cvMoveWindow("video", 30, 0);
	//cvMoveWindow("background", 360, 0);
	//cvMoveWindow("foreground", 690, 0);

	if(bReadFromFile)
	{
        pCapture=cvCaptureFromFile(fileName);
		pCapture1=cvCaptureFromFile(fileName1);
	}
	else
	{
        pCapture = cvCaptureFromCAM(0);
        pCapture1= cvCaptureFromCAM(1);
	}

	//逐帧读取视频
	while((pFrame = cvQueryFrame( pCapture ))&&(pFrame1 = cvQueryFrame( pCapture1 )))
	{
		
		nFrmNum++;
		//如果是第一帧，需要申请内存，并初始化
		if(nFrmNum == 1)
		{
			pBkImg = cvCreateImage(cvSize(pFrame->width, pFrame->height),IPL_DEPTH_8U,1);
			pFrameImg = cvCreateImage(cvSize(pFrame->width, pFrame->height),IPL_DEPTH_8U,1);

			pFrImg = cvCreateImage(cvSize(pFrame->width, pFrame->height),IPL_DEPTH_8U,3);
			pEndImg=cvCreateImage(cvSize(pFrame->width, pFrame->height),IPL_DEPTH_8U,3);
			pBkMat = cvCreateMat(pFrame->height, pFrame->width, CV_32FC1);
			pEndMat = cvCreateMat(pFrame->height, pFrame->width, CV_32FC1);
			pFrMat = cvCreateMat(pFrame->height, pFrame->width,CV_32FC1);

			//转化成单通道图像再处理
			cvCvtColor(pFrame, pBkImg, CV_BGR2GRAY);
			cvCvtColor(pFrame, pFrameImg, CV_BGR2GRAY);

			cvConvert(pFrameImg, pFrMat);
			cvConvert(pFrameImg, pEndMat);
			cvConvert(pFrameImg, pBkMat);

			//meanshift参数
			minX=pFrame->width;
			minY=pFrame->height;
			maxX=0;
			maxY=0;
			///////////////////////////////////一起初始化//////////////////////////////////////////////
			pBkImg1 = cvCreateImage(cvSize(pFrame1->width, pFrame1->height),IPL_DEPTH_8U,1);
			pFrameImg1 = cvCreateImage(cvSize(pFrame1->width, pFrame1->height),IPL_DEPTH_8U,1);

			pFrImg1 = cvCreateImage(cvSize(pFrame1->width, pFrame1->height),IPL_DEPTH_8U,3);
			pEndImg1=cvCreateImage(cvSize(pFrame1->width, pFrame1->height),IPL_DEPTH_8U,3);
			pBkMat1 = cvCreateMat(pFrame1->height, pFrame1->width, CV_32FC1);
			pEndMat1 = cvCreateMat(pFrame1->height, pFrame1->width, CV_32FC1);
			pFrMat1 = cvCreateMat(pFrame1->height, pFrame1->width,CV_32FC1);

			//转化成单通道图像再处理
			cvCvtColor(pFrame1, pBkImg1, CV_BGR2GRAY);
			cvCvtColor(pFrame1, pFrameImg1, CV_BGR2GRAY);

			cvConvert(pFrameImg1, pFrMat1);
			cvConvert(pFrameImg1, pEndMat1);
			cvConvert(pFrameImg1, pBkMat1);

			//meanshift参数
			minX1=pFrame1->width;
			minY1=pFrame1->height;
			maxX1=0;
			maxY1=0;
		}
		else
		{
			cvCvtColor(pFrame,pFrameImg, CV_BGR2GRAY);
			cvConvert(pFrameImg, pFrMat);
			//高斯滤波先，以平滑图像
			cvSmooth(pFrMat, pFrMat, CV_GAUSSIAN, 3, 0, 0);

			//当前帧跟背景图相减
			cvAbsDiff(pFrMat, pBkMat,pEndMat);
			//cvConvert(pEndMat,pFrImg);
			//二值化前景图
			cvThreshold(pEndMat,pFrameImg,60,255.0,CV_THRESH_BINARY);

			//进行形态学滤波，去掉噪音  
			cvDilate(pFrameImg, pFrameImg, 0, 1);
			cvErode(pFrameImg, pFrameImg, 0, 1);
			cvErode(pFrameImg, pFrameImg, 0, 1);
			cvDilate(pFrameImg, pFrameImg, 0, 1);


			//还原处理
			for(int i=0;i<pFrame->height;++i)
			{
				for(int j=0;j<pFrame->width;++j)
				{
					if(((uchar *)(pFrameImg->imageData + i*pFrameImg->widthStep))[j]!=0)
					{
						((uchar *)(pEndImg->imageData + i*pEndImg->widthStep))[j*pEndImg->nChannels + 0]=
							((uchar *)(pFrame->imageData + i*pFrame->widthStep))[j*pFrame->nChannels + 0]; 
						((uchar *)(pEndImg->imageData + i*pEndImg->widthStep))[j*pEndImg->nChannels + 1]=
							((uchar *)(pFrame->imageData + i*pFrame->widthStep))[j*pFrame->nChannels + 1]; 
						((uchar *)(pEndImg->imageData + i*pEndImg->widthStep))[j*pEndImg->nChannels + 2]=
							((uchar *)(pFrame->imageData + i*pFrame->widthStep))[j*pFrame->nChannels + 2];
						if(minX>j) minX=j;
						if(minY>i) minY=i;
						if(maxX<j) maxX=j;
						if(maxY<i) maxY=i;
					}
					else
					{
						((uchar *)(pEndImg->imageData + i*pEndImg->widthStep))[j*pEndImg->nChannels + 0]=255; 
						((uchar *)(pEndImg->imageData + i*pEndImg->widthStep))[j*pEndImg->nChannels + 1]=255; 
						((uchar *)(pEndImg->imageData + i*pEndImg->widthStep))[j*pEndImg->nChannels + 2]=255;
					}
				}
			}
			pEndImg->origin=pFrame->origin;
			//更新背景
			cvRunningAvg(pFrMat, pBkMat, 0.005, 0);
			//将背景转化为图像格式，用以显示
			cvConvert(pBkMat, pBkImg);
			
			//显示图像
			pBkImg->origin=pFrame->origin;

			//cvShowImage("video", pFrame);
			//cvShowImage("background",pBkImg);
			//cvShowImage("foreground", pEndImg);


			//如果有按键事件，则跳出循环
			//此等待也为cvShowImage函数提供时间完成显示
			//等待时间可以根据CPU速度调整
			if( cvWaitKey(3)=='q')
			{
				cvSaveImage("L.jpg",pFrame);
				cvSaveImage("R.jpg",pEndImg);
				cvSaveImage("End.jpg",pFrImg);
				break;
			}
			cvCopyImage(pFrame,pFrImg);


			if(nFrmNum ==2)
			{
				selection.x=minX;
				selection.y=minY;
				selection.width=maxX-minX;
				selection.height=maxY-minY;

				if(selection.width<0||selection.height<0) nFrmNum=1;

				selection.x = MAX( selection.x, 0 );
				selection.y = MAX( selection.y, 0 );
				selection.width = MIN( selection.width, pFrame->width );
				selection.height = MIN( selection.height,pFrame->height );

				track_object=-1;

			}
			
			////////////////////////////////////////////////////////////////
			cvCvtColor(pFrame1,pFrameImg1, CV_BGR2GRAY);
			cvConvert(pFrameImg1, pFrMat1);
			//高斯滤波先，以平滑图像
			cvSmooth(pFrMat1, pFrMat1, CV_GAUSSIAN, 3, 0, 0);

			//当前帧跟背景图相减
			cvAbsDiff(pFrMat1, pBkMat1,pEndMat1);
			//cvConvert(pEndMat,pFrImg);
			//二值化前景图
			cvThreshold(pEndMat1,pFrameImg1,60,255.0,CV_THRESH_BINARY);

			//进行形态学滤波，去掉噪音  
			cvDilate(pFrameImg1, pFrameImg1, 0, 1);
			cvErode(pFrameImg1, pFrameImg1, 0, 1);
			cvErode(pFrameImg1, pFrameImg1, 0, 1);
			cvDilate(pFrameImg1, pFrameImg1, 0, 1);


			//还原处理
			for(int i=0;i<pFrame1->height;++i)
			{
				for(int j=0;j<pFrame1->width;++j)
				{
					if(((uchar *)(pFrameImg1->imageData + i*pFrameImg->widthStep))[j]!=0)
					{
						((uchar *)(pEndImg1->imageData + i*pEndImg1->widthStep))[j*pEndImg1->nChannels + 0]=
							((uchar *)(pFrame1->imageData + i*pFrame1->widthStep))[j*pFrame1->nChannels + 0]; 
						((uchar *)(pEndImg1->imageData + i*pEndImg1->widthStep))[j*pEndImg1->nChannels + 1]=
							((uchar *)(pFrame1->imageData + i*pFrame1->widthStep))[j*pFrame1->nChannels + 1]; 
						((uchar *)(pEndImg1->imageData + i*pEndImg1->widthStep))[j*pEndImg1->nChannels + 2]=
							((uchar *)(pFrame1->imageData + i*pFrame1->widthStep))[j*pFrame1->nChannels + 2];
						if(minX1>j) minX1=j;
						if(minY1>i) minY1=i;
						if(maxX1<j) maxX1=j;
						if(maxY1<i) maxY1=i;
					}
					else
					{
						((uchar *)(pEndImg1->imageData + i*pEndImg1->widthStep))[j*pEndImg1->nChannels + 0]=255; 
						((uchar *)(pEndImg1->imageData + i*pEndImg1->widthStep))[j*pEndImg1->nChannels + 1]=255; 
						((uchar *)(pEndImg1->imageData + i*pEndImg1->widthStep))[j*pEndImg1->nChannels + 2]=255;
					}
				}
			}
			pEndImg1->origin=pFrame1->origin;
			//更新背景
			cvRunningAvg(pFrMat1, pBkMat1, 0.005, 0);
			//将背景转化为图像格式，用以显示
			cvConvert(pBkMat1, pBkImg1);
			
			//显示图像
			pBkImg1->origin=pFrame1->origin;

			
			cvCopyImage(pFrame1,pFrImg1);


			if(nFrmNum ==2)
			{
				selection1.x=minX1;
				selection1.y=minY1;
				selection1.width=maxX1-minX1;
				selection1.height=maxY1-minY1;

				if(selection1.width<0||selection1.height<0) nFrmNum=1;

				selection1.x = MAX( selection1.x, 0 );
				selection1.y = MAX( selection1.y, 0 );
				selection1.width = MIN( selection1.width, pFrame1->width );
				selection1.height = MIN( selection1.height,pFrame1->height );

				track_object1=-1;

			}
			if(nFrmNum >=2)
			{
				MeanShift(pFrame);
				MeanShift1(pFrame1);
			}
			//printf("%d   %d  &  %d   %d\n",selection.x,selection.y,selection.width,selection.width);
			//if(nFrmNum >=2) MeanShift(pFrame);
			//printf("%d   %d  &  %d   %d\n",selection.x,selection.y,selection.width,selection.width);

		}
		////////////////////////////////////////////////////////////////////////////////
		
	}
	cvDestroyWindow("CamShiftDemo");
	//销毁窗口
	cvReleaseCapture(&pCapture);
	cvDestroyWindow("video");
	cvDestroyWindow("background");
	cvDestroyWindow("foreground");

	//释放图像和矩阵
	cvReleaseImage(&pFrImg);
	cvReleaseImage(&pBkImg);

	cvReleaseMat(&pFrMat);
	cvReleaseMat(&pFrMat);
	cvReleaseMat(&pBkMat);


	/*//Sift s;//这里取消
	//cout<<"SIFT end!"<<endl;
	cvWaitKey(0);*/

	cvDestroyWindow("CamShiftDemo1");
	//销毁窗口
	cvReleaseCapture(&pCapture1);
	cvDestroyWindow("video");
	cvDestroyWindow("background");
	cvDestroyWindow("foreground");

	//释放图像和矩阵
	cvReleaseImage(&pFrImg1);
	cvReleaseImage(&pBkImg1);

	cvReleaseMat(&pFrMat1);
	cvReleaseMat(&pFrMat1);
	cvReleaseMat(&pBkMat1);


}

CvScalar FrameDifference::hsv2rgb( float hue )//用于将Hue量转换成RGB量
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

int FrameDifference::MeanShift(IplImage* frame)
{
	NumFrames++;
	float hranges_arr[]= {0,180};//像素值的范围
	float *hranges = hranges_arr;//用于初始化CvHistogram类

	int i, bin_w;
	if( !frame )
		return -1;
	//else track_object=-1;

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

		if( track_object<0)//如果需要跟踪的物体还没有进行属性提取，则进行选取框类的图像属性提取
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
		//cvNamedWindow("df",1);
		//cvShowImage("df",backproject);


		cvAnd( backproject, mask, backproject, 0 );//得到反向投影图mask内的内容
		cvCamShift( backproject, track_window,cvTermCriteria( CV_TERMCRIT_EPS | CV_TERMCRIT_ITER, 10, 1 ),&track_comp, &track_box );//使用MeanShift算法对backproject中的内容进行搜索，返回跟踪结果
		track_window = track_comp.rect;//得到跟踪结果的矩形框
		//puts("df");
		if( backproject_mode )
			cvCvtColor( backproject, image, CV_GRAY2BGR ); // 显示模式
		if( image->origin )
			track_box.angle = -track_box.angle;
		cvEllipseBox( image, track_box, CV_RGB(255,0,0), 3, CV_AA, 0 );//画出跟踪结果的位置
	}
	cvShowImage( "CamShiftDemo", image );//显示视频和直方图
	//cvShowImage( "Histogram", histimg );

	if(count1%30 == 0)
	{
		char str[100];
		sprintf(str, "%d.jpg", count1);
		cvSaveImage(str,image);
	}
	//cvShowImage( "Histogram", histimg );
	//puts("hehe");
	count1 += 1;
}
///////////////////////////////////////////////////////////////////////////
int FrameDifference::MeanShift1(IplImage* frame)
{
	NumFrames1++;
	float hranges_arr[]= {0,180};//像素值的范围
	float *hranges = hranges_arr;//用于初始化CvHistogram类

	int i, bin_w;
	if( !frame )
		return -1;
	//else track_object=-1;

	if(NumFrames1==1)//刚开始先建立一些缓冲区
	{ 
		image1 = cvCreateImage( cvGetSize(frame), 8, 3 );//
		image1->origin = frame->origin;//origin为0表示顶左结构，即图像的原点是左上角，如果为1为左下角

		hsv1 = cvCreateImage( cvGetSize(frame), 8, 3 );
		hue1 = cvCreateImage( cvGetSize(frame), 8, 1 );

		mask1 = cvCreateImage( cvGetSize(frame), 8, 1 );//分配掩膜图像空间

		backproject1 = cvCreateImage( cvGetSize(frame), 8, 1 );//分配反向投影图空间，大小一样，单通道

		hist1 = cvCreateHist( 1, &hdims1, CV_HIST_ARRAY, &hranges, 1 ); //分配建立直方图空间

		histimg1 = cvCreateImage( cvSize(320,200), 8, 3);//分配用于画直方图的空间
		cvZero( histimg1 );//背景为黑色
	}

	cvCopy( frame, image1, 0 );
	cvCvtColor( image1, hsv1, CV_BGR2HSV ); // 把图像从RGB表色系转为HSV表色系

	if(track_object1)// 如果当前有需要跟踪的物体
	{
		int _vmin = vmin1, _vmax = vmax1;
		//inRange函数的功能是检查输入数组每个元素大小是否在2个给定数值之间，可以有多通道,mask保存0通道的最小值，也就是h分量，左边三个最小到右边三个最大
		cvInRangeS( hsv1, cvScalar(0,smin1,MIN(_vmin,_vmax),0),cvScalar(180,256,MAX(_vmin,_vmax),0), mask1 ); //制作掩膜板，只处理像素值为H：0~180，S：smin~256，V：vmin~vmax之间的部分
		cvSplit( hsv1, hue1, 0, 0, 0 ); // 取得H分量

		if( track_object1<0)//如果需要跟踪的物体还没有进行属性提取，则进行选取框类的图像属性提取
		{
			float max_val = 0.f;
			cvSetImageROI( hue1, selection1 ); // 设置原选择框
			cvSetImageROI( mask1, selection1 ); // 设置Mask的选择框
			//calcHist()函数第一个参数为输入矩阵序列，第2个参数表示输入的矩阵数目，第3个参数表示将被计算直方图维数通道的列表，第4个参数表示可选的掩码函数
			//第5个参数表示输出直方图，第6个参数表示直方图的维数，第7个参数为每一维直方图数组的大小，第8个参数为每一维直方图bin的边界
			cvCalcHist( &hue1, hist1, 0, mask1 ); // 得到选择框内且满足掩膜板内的直方图

			cvGetMinMaxHistValue( hist1, 0, &max_val, 0, 0 ); 
			cvConvertScale( hist1->bins, hist1->bins, max_val ? 255. / max_val : 0., 0 ); // 对直方图转为0~255

			cvResetImageROI( hue1 ); // remove ROI
			cvResetImageROI( mask1 );
			track_window1 = selection1;
			track_object1 = 1;//只要鼠标选完区域松开后，且没有按键盘清0键'c'，则trackObject一直保持为1，因此该if函数只能执行一次，除非重新选择跟踪区域

			cvZero( histimg1 );
			//histing是一个200*300的矩阵，hsize应该是每一个bin的宽度，也就是histing矩阵能分出几个bin出来
			bin_w = histimg1->width / hdims1;
			for( i = 0; i < hdims1; i++ )
			{
				int val = cvRound( cvGetReal1D(hist1->bins,i)*histimg1->height/255 );
				CvScalar color = hsv2rgb(i*180.f/hdims1);

				cvRectangle( histimg1, cvPoint(i*bin_w,histimg1->height),
					cvPoint((i+1)*bin_w,histimg1->height - val),color, -1, 8, 0 );//画直方图到图像空间
			}
		}

		cvCalcBackProject( &hue1, backproject1, hist1 ); // 得到hue的反向投影图
		//cvNamedWindow("df1",1);
		//cvShowImage("df1",backproject1);


		cvAnd( backproject1, mask1, backproject1, 0 );//得到反向投影图mask内的内容
		cvCamShift( backproject1, track_window1,cvTermCriteria( CV_TERMCRIT_EPS | CV_TERMCRIT_ITER, 10, 1 ),&track_comp1, &track_box1 );//使用MeanShift算法对backproject中的内容进行搜索，返回跟踪结果
		track_window1 = track_comp1.rect;//得到跟踪结果的矩形框
		//puts("df");
		if( backproject_mode1 )
			cvCvtColor( backproject1, image1, CV_GRAY2BGR ); // 显示模式
		if( image1->origin )
			track_box1.angle = -track_box1.angle;
		cvEllipseBox( image1, track_box1, CV_RGB(255,0,0), 3, CV_AA, 0 );//画出跟踪结果的位置
	}
	cvShowImage( "CamShiftDemo1", image1 );//显示视频和直方图
	//cvShowImage( "Histogram", histimg );
}