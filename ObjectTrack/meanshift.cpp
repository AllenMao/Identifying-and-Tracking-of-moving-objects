#include "MeanShift.h"

IplImage *image = 0, *hsv = 0, *hue = 0, *mask = 0, *backproject = 0, *histimg = 0;//��HSV�е�Hue�������и���
CvHistogram *hist = 0;//ֱ��ͼ��

int backproject_mode = 0;//��ʾ�Ƿ�Ҫ���뷴��ͶӰģʽ��ture��ʾ׼�����뷴��ͶӰģʽ
int select_object = 0;//�����Ƿ���ѡҪ���ٵĳ�ʼĿ�꣬true��ʾ���������ѡ��
int track_object = 0;//�������Ŀ����Ŀ
int show_hist = 1;//�Ƿ���ʾֱ��ͼ
CvPoint origin;//���ڱ������ѡ���һ�ε���ʱ���λ��
CvRect selection;//���ڱ������ѡ��ľ��ο�
CvRect track_window;
CvBox2D track_box; // Meanshift�����㷨���ص�Box��
CvConnectedComp track_comp;

int hdims = 50; // ����ֱ��ͼbins�ĸ�����Խ��Խ��ȷ
float hranges_arr[] = {0,180};//����ֵ�ķ�Χ
float* hranges = hranges_arr;//���ڳ�ʼ��CvHistogram��
int NumFrames=0;

CvScalar hsv2rgb( float hue )//���ڽ�Hue��ת����RGB��
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

	return cvScalar(rgb[2], rgb[1], rgb[0],0);//���ض�Ӧ����ɫֵ
}

int MeanShift(CvRect selection,IplImage* frame)
{
	int vmin = 10, vmax = 256, smin = 30;
	NumFrames++;
	cvNamedWindow( "CamShiftDemo", 1 );//������Ƶ����

	cvCreateTrackbar( "Vmin", "CamShiftDemo", &vmin, 256, 0 );//����������
	cvCreateTrackbar( "Vmax", "CamShiftDemo", &vmax, 256, 0 );
	cvCreateTrackbar( "Smin", "CamShiftDemo", &smin, 256, 0 );

	int i, bin_w, c;
	if( !frame )
		return -1;
	else track_object=-1;

	if(NumFrames==1)//�տ�ʼ�Ƚ���һЩ������
	{ 
		image = cvCreateImage( cvGetSize(frame), 8, 3 );//
		image->origin = frame->origin;//originΪ0��ʾ����ṹ����ͼ���ԭ�������Ͻǣ����Ϊ1Ϊ���½�

		hsv = cvCreateImage( cvGetSize(frame), 8, 3 );
		hue = cvCreateImage( cvGetSize(frame), 8, 1 );

		mask = cvCreateImage( cvGetSize(frame), 8, 1 );//������Ĥͼ��ռ�

		backproject = cvCreateImage( cvGetSize(frame), 8, 1 );//���䷴��ͶӰͼ�ռ䣬��Сһ������ͨ��

		hist = cvCreateHist( 1, &hdims, CV_HIST_ARRAY, &hranges, 1 ); //���佨��ֱ��ͼ�ռ�

		histimg = cvCreateImage( cvSize(320,200), 8, 3);//�������ڻ�ֱ��ͼ�Ŀռ�
		cvZero( histimg );//����Ϊ��ɫ
	}

	cvCopy( frame, image, 0 );
	cvCvtColor( image, hsv, CV_BGR2HSV ); // ��ͼ���RGB��ɫϵתΪHSV��ɫϵ

	if(track_object)// �����ǰ����Ҫ���ٵ�����
	{
		int _vmin = vmin, _vmax = vmax;
		//inRange�����Ĺ����Ǽ����������ÿ��Ԫ�ش�С�Ƿ���2��������ֵ֮�䣬�����ж�ͨ��,mask����0ͨ������Сֵ��Ҳ����h���������������С���ұ��������
		cvInRangeS( hsv, cvScalar(0,smin,MIN(_vmin,_vmax),0),cvScalar(180,256,MAX(_vmin,_vmax),0), mask ); //������Ĥ�壬ֻ��������ֵΪH��0~180��S��smin~256��V��vmin~vmax֮��Ĳ���
		cvSplit( hsv, hue, 0, 0, 0 ); // ȡ��H����

		if( track_object < 0 )//�����Ҫ���ٵ����廹û�н���������ȡ�������ѡȡ�����ͼ��������ȡ
		{
			float max_val = 0.f;
			cvSetImageROI( hue, selection ); // ����ԭѡ���
			cvSetImageROI( mask, selection ); // ����Mask��ѡ���
			//calcHist()������һ������Ϊ����������У���2��������ʾ����ľ�����Ŀ����3��������ʾ��������ֱ��ͼά��ͨ�����б���4��������ʾ��ѡ�����뺯��
			//��5��������ʾ���ֱ��ͼ����6��������ʾֱ��ͼ��ά������7������Ϊÿһάֱ��ͼ����Ĵ�С����8������Ϊÿһάֱ��ͼbin�ı߽�
			cvCalcHist( &hue, hist, 0, mask ); // �õ�ѡ�������������Ĥ���ڵ�ֱ��ͼ

			cvGetMinMaxHistValue( hist, 0, &max_val, 0, 0 ); 
			cvConvertScale( hist->bins, hist->bins, max_val ? 255. / max_val : 0., 0 ); // ��ֱ��ͼתΪ0~255

			cvResetImageROI( hue ); // remove ROI
			cvResetImageROI( mask );
			track_window = selection;
			track_object = 1;//ֻҪ���ѡ�������ɿ�����û�а�������0��'c'����trackObjectһֱ����Ϊ1����˸�if����ֻ��ִ��һ�Σ���������ѡ���������

			cvZero( histimg );
			//histing��һ��200*300�ľ���hsizeӦ����ÿһ��bin�Ŀ�ȣ�Ҳ����histing�����ֳܷ�����bin����
			bin_w = histimg->width / hdims;
			for( i = 0; i < hdims; i++ )
			{
				int val = cvRound( cvGetReal1D(hist->bins,i)*histimg->height/255 );
				CvScalar color = hsv2rgb(i*180.f/hdims);

				cvRectangle( histimg, cvPoint(i*bin_w,histimg->height),
					cvPoint((i+1)*bin_w,histimg->height - val),color, -1, 8, 0 );//��ֱ��ͼ��ͼ��ռ�
			}
		}

		cvCalcBackProject( &hue, backproject, hist ); // �õ�hue�ķ���ͶӰͼ
		cvNamedWindow("df",1);
		cvShowImage("df",backproject);


		cvAnd( backproject, mask, backproject, 0 );//�õ�����ͶӰͼmask�ڵ�����
		cvCamShift( backproject, track_window,cvTermCriteria( CV_TERMCRIT_EPS | CV_TERMCRIT_ITER, 10, 1 ),&track_comp, &track_box );//ʹ��MeanShift�㷨��backproject�е����ݽ������������ظ��ٽ��
		track_window = track_comp.rect;//�õ����ٽ���ľ��ο�

		if( backproject_mode )
			cvCvtColor( backproject, image, CV_GRAY2BGR ); // ��ʾģʽ
		if( image->origin )
			track_box.angle = -track_box.angle;
		cvEllipseBox( image, track_box, CV_RGB(255,0,0), 3, CV_AA, 0 );//�������ٽ����λ��
	}

	if( select_object && selection.width > 0 && selection.height > 0 )//�������������ѡ�񣬻���ѡ���
	{
		cvSetImageROI( image, selection );
		cvXorS( image, cvScalarAll(255), image, 0 );
		cvResetImageROI( image );
	}
	

	cvShowImage( "CamShiftDemo", image );//��ʾ��Ƶ��ֱ��ͼ
	cvShowImage( "Histogram", histimg );
	cvWaitKey(1000);
}

