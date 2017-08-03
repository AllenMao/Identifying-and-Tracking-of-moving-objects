/**************************************************
* ������ģ���˶�������
*      
**************************************************/
#pragma once

#include"Main.h"

int count1=0;

FrameDifference::FrameDifference(bool bReadFromFile, char *fileName, char *fileName1)
{
	/*meanshift-begin*/
	image = 0, hsv = 0, hue = 0, mask = 0, backproject = 0, histimg = 0;//��HSV�е�Hue�������и���
	hist = 0;//ֱ��ͼ��

	backproject_mode = 0;//��ʾ�Ƿ�Ҫ���뷴��ͶӰģʽ��ture��ʾ׼�����뷴��ͶӰģʽ
	select_object = 0;//�����Ƿ���ѡҪ���ٵĳ�ʼĿ�꣬true��ʾ���������ѡ��
	track_object = 0;//�������Ŀ����Ŀ
	show_hist = 1;//�Ƿ���ʾֱ��ͼ
	origin;//���ڱ������ѡ���һ�ε���ʱ���λ��
	selection;//���ڱ������ѡ��ľ��ο�
	track_window;
	track_box; // Meanshift�����㷨���ص�Box��
	track_comp;

	hdims = 50; // ����ֱ��ͼbins�ĸ�����Խ��Խ��ȷ

	NumFrames=0;
	vmin = 10, vmax = 256, smin = 30;
	/*meanshift-end*/
	/*meanshift1-begin*/
	image1 = 0, hsv1 = 0, hue1 = 0, mask1 = 0, backproject1 = 0, histimg1 = 0;//��HSV�е�Hue�������и���
	hist1 = 0;//ֱ��ͼ��

	backproject_mode1 = 0;//��ʾ�Ƿ�Ҫ���뷴��ͶӰģʽ��ture��ʾ׼�����뷴��ͶӰģʽ
	select_object1 = 0;//�����Ƿ���ѡҪ���ٵĳ�ʼĿ�꣬true��ʾ���������ѡ��
	track_object1 = 0;//�������Ŀ����Ŀ
	show_hist1 = 1;//�Ƿ���ʾֱ��ͼ
	origin1;//���ڱ������ѡ���һ�ε���ʱ���λ��
	selection1;//���ڱ������ѡ��ľ��ο�
	track_window1;
	track_box1; // Meanshift�����㷨���ص�Box��
	track_comp1;

	hdims1 = 50; // ����ֱ��ͼbins�ĸ�����Խ��Խ��ȷ

	NumFrames1=0;
	vmin1 = 10, vmax1 = 256, smin1 = 30;
	/*meanshift1-end*/

	//����IplImageָ��
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

	//����IplImageָ��1
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
	//��������
	//cvNamedWindow("video", 1);
	//cvNamedWindow("background",1);
	//cvNamedWindow("foreground",1);
	/*meanshift*/
	cvNamedWindow( "CamShiftDemo", 1 );//������Ƶ����
	cvNamedWindow( "CamShiftDemo1", 1 );//������Ƶ����
	cvMoveWindow("CamShiftDemo", 30, 0);
	cvMoveWindow("CamShiftDemo1", 360, 0);


	//ʹ������������
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

	//��֡��ȡ��Ƶ
	while((pFrame = cvQueryFrame( pCapture ))&&(pFrame1 = cvQueryFrame( pCapture1 )))
	{
		
		nFrmNum++;
		//����ǵ�һ֡����Ҫ�����ڴ棬����ʼ��
		if(nFrmNum == 1)
		{
			pBkImg = cvCreateImage(cvSize(pFrame->width, pFrame->height),IPL_DEPTH_8U,1);
			pFrameImg = cvCreateImage(cvSize(pFrame->width, pFrame->height),IPL_DEPTH_8U,1);

			pFrImg = cvCreateImage(cvSize(pFrame->width, pFrame->height),IPL_DEPTH_8U,3);
			pEndImg=cvCreateImage(cvSize(pFrame->width, pFrame->height),IPL_DEPTH_8U,3);
			pBkMat = cvCreateMat(pFrame->height, pFrame->width, CV_32FC1);
			pEndMat = cvCreateMat(pFrame->height, pFrame->width, CV_32FC1);
			pFrMat = cvCreateMat(pFrame->height, pFrame->width,CV_32FC1);

			//ת���ɵ�ͨ��ͼ���ٴ���
			cvCvtColor(pFrame, pBkImg, CV_BGR2GRAY);
			cvCvtColor(pFrame, pFrameImg, CV_BGR2GRAY);

			cvConvert(pFrameImg, pFrMat);
			cvConvert(pFrameImg, pEndMat);
			cvConvert(pFrameImg, pBkMat);

			//meanshift����
			minX=pFrame->width;
			minY=pFrame->height;
			maxX=0;
			maxY=0;
			///////////////////////////////////һ���ʼ��//////////////////////////////////////////////
			pBkImg1 = cvCreateImage(cvSize(pFrame1->width, pFrame1->height),IPL_DEPTH_8U,1);
			pFrameImg1 = cvCreateImage(cvSize(pFrame1->width, pFrame1->height),IPL_DEPTH_8U,1);

			pFrImg1 = cvCreateImage(cvSize(pFrame1->width, pFrame1->height),IPL_DEPTH_8U,3);
			pEndImg1=cvCreateImage(cvSize(pFrame1->width, pFrame1->height),IPL_DEPTH_8U,3);
			pBkMat1 = cvCreateMat(pFrame1->height, pFrame1->width, CV_32FC1);
			pEndMat1 = cvCreateMat(pFrame1->height, pFrame1->width, CV_32FC1);
			pFrMat1 = cvCreateMat(pFrame1->height, pFrame1->width,CV_32FC1);

			//ת���ɵ�ͨ��ͼ���ٴ���
			cvCvtColor(pFrame1, pBkImg1, CV_BGR2GRAY);
			cvCvtColor(pFrame1, pFrameImg1, CV_BGR2GRAY);

			cvConvert(pFrameImg1, pFrMat1);
			cvConvert(pFrameImg1, pEndMat1);
			cvConvert(pFrameImg1, pBkMat1);

			//meanshift����
			minX1=pFrame1->width;
			minY1=pFrame1->height;
			maxX1=0;
			maxY1=0;
		}
		else
		{
			cvCvtColor(pFrame,pFrameImg, CV_BGR2GRAY);
			cvConvert(pFrameImg, pFrMat);
			//��˹�˲��ȣ���ƽ��ͼ��
			cvSmooth(pFrMat, pFrMat, CV_GAUSSIAN, 3, 0, 0);

			//��ǰ֡������ͼ���
			cvAbsDiff(pFrMat, pBkMat,pEndMat);
			//cvConvert(pEndMat,pFrImg);
			//��ֵ��ǰ��ͼ
			cvThreshold(pEndMat,pFrameImg,60,255.0,CV_THRESH_BINARY);

			//������̬ѧ�˲���ȥ������  
			cvDilate(pFrameImg, pFrameImg, 0, 1);
			cvErode(pFrameImg, pFrameImg, 0, 1);
			cvErode(pFrameImg, pFrameImg, 0, 1);
			cvDilate(pFrameImg, pFrameImg, 0, 1);


			//��ԭ����
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
			//���±���
			cvRunningAvg(pFrMat, pBkMat, 0.005, 0);
			//������ת��Ϊͼ���ʽ��������ʾ
			cvConvert(pBkMat, pBkImg);
			
			//��ʾͼ��
			pBkImg->origin=pFrame->origin;

			//cvShowImage("video", pFrame);
			//cvShowImage("background",pBkImg);
			//cvShowImage("foreground", pEndImg);


			//����а����¼���������ѭ��
			//�˵ȴ�ҲΪcvShowImage�����ṩʱ�������ʾ
			//�ȴ�ʱ����Ը���CPU�ٶȵ���
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
			//��˹�˲��ȣ���ƽ��ͼ��
			cvSmooth(pFrMat1, pFrMat1, CV_GAUSSIAN, 3, 0, 0);

			//��ǰ֡������ͼ���
			cvAbsDiff(pFrMat1, pBkMat1,pEndMat1);
			//cvConvert(pEndMat,pFrImg);
			//��ֵ��ǰ��ͼ
			cvThreshold(pEndMat1,pFrameImg1,60,255.0,CV_THRESH_BINARY);

			//������̬ѧ�˲���ȥ������  
			cvDilate(pFrameImg1, pFrameImg1, 0, 1);
			cvErode(pFrameImg1, pFrameImg1, 0, 1);
			cvErode(pFrameImg1, pFrameImg1, 0, 1);
			cvDilate(pFrameImg1, pFrameImg1, 0, 1);


			//��ԭ����
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
			//���±���
			cvRunningAvg(pFrMat1, pBkMat1, 0.005, 0);
			//������ת��Ϊͼ���ʽ��������ʾ
			cvConvert(pBkMat1, pBkImg1);
			
			//��ʾͼ��
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
	//���ٴ���
	cvReleaseCapture(&pCapture);
	cvDestroyWindow("video");
	cvDestroyWindow("background");
	cvDestroyWindow("foreground");

	//�ͷ�ͼ��;���
	cvReleaseImage(&pFrImg);
	cvReleaseImage(&pBkImg);

	cvReleaseMat(&pFrMat);
	cvReleaseMat(&pFrMat);
	cvReleaseMat(&pBkMat);


	/*//Sift s;//����ȡ��
	//cout<<"SIFT end!"<<endl;
	cvWaitKey(0);*/

	cvDestroyWindow("CamShiftDemo1");
	//���ٴ���
	cvReleaseCapture(&pCapture1);
	cvDestroyWindow("video");
	cvDestroyWindow("background");
	cvDestroyWindow("foreground");

	//�ͷ�ͼ��;���
	cvReleaseImage(&pFrImg1);
	cvReleaseImage(&pBkImg1);

	cvReleaseMat(&pFrMat1);
	cvReleaseMat(&pFrMat1);
	cvReleaseMat(&pBkMat1);


}

CvScalar FrameDifference::hsv2rgb( float hue )//���ڽ�Hue��ת����RGB��
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

int FrameDifference::MeanShift(IplImage* frame)
{
	NumFrames++;
	float hranges_arr[]= {0,180};//����ֵ�ķ�Χ
	float *hranges = hranges_arr;//���ڳ�ʼ��CvHistogram��

	int i, bin_w;
	if( !frame )
		return -1;
	//else track_object=-1;

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

		if( track_object<0)//�����Ҫ���ٵ����廹û�н���������ȡ�������ѡȡ�����ͼ��������ȡ
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
		//cvNamedWindow("df",1);
		//cvShowImage("df",backproject);


		cvAnd( backproject, mask, backproject, 0 );//�õ�����ͶӰͼmask�ڵ�����
		cvCamShift( backproject, track_window,cvTermCriteria( CV_TERMCRIT_EPS | CV_TERMCRIT_ITER, 10, 1 ),&track_comp, &track_box );//ʹ��MeanShift�㷨��backproject�е����ݽ������������ظ��ٽ��
		track_window = track_comp.rect;//�õ����ٽ���ľ��ο�
		//puts("df");
		if( backproject_mode )
			cvCvtColor( backproject, image, CV_GRAY2BGR ); // ��ʾģʽ
		if( image->origin )
			track_box.angle = -track_box.angle;
		cvEllipseBox( image, track_box, CV_RGB(255,0,0), 3, CV_AA, 0 );//�������ٽ����λ��
	}
	cvShowImage( "CamShiftDemo", image );//��ʾ��Ƶ��ֱ��ͼ
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
	float hranges_arr[]= {0,180};//����ֵ�ķ�Χ
	float *hranges = hranges_arr;//���ڳ�ʼ��CvHistogram��

	int i, bin_w;
	if( !frame )
		return -1;
	//else track_object=-1;

	if(NumFrames1==1)//�տ�ʼ�Ƚ���һЩ������
	{ 
		image1 = cvCreateImage( cvGetSize(frame), 8, 3 );//
		image1->origin = frame->origin;//originΪ0��ʾ����ṹ����ͼ���ԭ�������Ͻǣ����Ϊ1Ϊ���½�

		hsv1 = cvCreateImage( cvGetSize(frame), 8, 3 );
		hue1 = cvCreateImage( cvGetSize(frame), 8, 1 );

		mask1 = cvCreateImage( cvGetSize(frame), 8, 1 );//������Ĥͼ��ռ�

		backproject1 = cvCreateImage( cvGetSize(frame), 8, 1 );//���䷴��ͶӰͼ�ռ䣬��Сһ������ͨ��

		hist1 = cvCreateHist( 1, &hdims1, CV_HIST_ARRAY, &hranges, 1 ); //���佨��ֱ��ͼ�ռ�

		histimg1 = cvCreateImage( cvSize(320,200), 8, 3);//�������ڻ�ֱ��ͼ�Ŀռ�
		cvZero( histimg1 );//����Ϊ��ɫ
	}

	cvCopy( frame, image1, 0 );
	cvCvtColor( image1, hsv1, CV_BGR2HSV ); // ��ͼ���RGB��ɫϵתΪHSV��ɫϵ

	if(track_object1)// �����ǰ����Ҫ���ٵ�����
	{
		int _vmin = vmin1, _vmax = vmax1;
		//inRange�����Ĺ����Ǽ����������ÿ��Ԫ�ش�С�Ƿ���2��������ֵ֮�䣬�����ж�ͨ��,mask����0ͨ������Сֵ��Ҳ����h���������������С���ұ��������
		cvInRangeS( hsv1, cvScalar(0,smin1,MIN(_vmin,_vmax),0),cvScalar(180,256,MAX(_vmin,_vmax),0), mask1 ); //������Ĥ�壬ֻ��������ֵΪH��0~180��S��smin~256��V��vmin~vmax֮��Ĳ���
		cvSplit( hsv1, hue1, 0, 0, 0 ); // ȡ��H����

		if( track_object1<0)//�����Ҫ���ٵ����廹û�н���������ȡ�������ѡȡ�����ͼ��������ȡ
		{
			float max_val = 0.f;
			cvSetImageROI( hue1, selection1 ); // ����ԭѡ���
			cvSetImageROI( mask1, selection1 ); // ����Mask��ѡ���
			//calcHist()������һ������Ϊ����������У���2��������ʾ����ľ�����Ŀ����3��������ʾ��������ֱ��ͼά��ͨ�����б���4��������ʾ��ѡ�����뺯��
			//��5��������ʾ���ֱ��ͼ����6��������ʾֱ��ͼ��ά������7������Ϊÿһάֱ��ͼ����Ĵ�С����8������Ϊÿһάֱ��ͼbin�ı߽�
			cvCalcHist( &hue1, hist1, 0, mask1 ); // �õ�ѡ�������������Ĥ���ڵ�ֱ��ͼ

			cvGetMinMaxHistValue( hist1, 0, &max_val, 0, 0 ); 
			cvConvertScale( hist1->bins, hist1->bins, max_val ? 255. / max_val : 0., 0 ); // ��ֱ��ͼתΪ0~255

			cvResetImageROI( hue1 ); // remove ROI
			cvResetImageROI( mask1 );
			track_window1 = selection1;
			track_object1 = 1;//ֻҪ���ѡ�������ɿ�����û�а�������0��'c'����trackObjectһֱ����Ϊ1����˸�if����ֻ��ִ��һ�Σ���������ѡ���������

			cvZero( histimg1 );
			//histing��һ��200*300�ľ���hsizeӦ����ÿһ��bin�Ŀ�ȣ�Ҳ����histing�����ֳܷ�����bin����
			bin_w = histimg1->width / hdims1;
			for( i = 0; i < hdims1; i++ )
			{
				int val = cvRound( cvGetReal1D(hist1->bins,i)*histimg1->height/255 );
				CvScalar color = hsv2rgb(i*180.f/hdims1);

				cvRectangle( histimg1, cvPoint(i*bin_w,histimg1->height),
					cvPoint((i+1)*bin_w,histimg1->height - val),color, -1, 8, 0 );//��ֱ��ͼ��ͼ��ռ�
			}
		}

		cvCalcBackProject( &hue1, backproject1, hist1 ); // �õ�hue�ķ���ͶӰͼ
		//cvNamedWindow("df1",1);
		//cvShowImage("df1",backproject1);


		cvAnd( backproject1, mask1, backproject1, 0 );//�õ�����ͶӰͼmask�ڵ�����
		cvCamShift( backproject1, track_window1,cvTermCriteria( CV_TERMCRIT_EPS | CV_TERMCRIT_ITER, 10, 1 ),&track_comp1, &track_box1 );//ʹ��MeanShift�㷨��backproject�е����ݽ������������ظ��ٽ��
		track_window1 = track_comp1.rect;//�õ����ٽ���ľ��ο�
		//puts("df");
		if( backproject_mode1 )
			cvCvtColor( backproject1, image1, CV_GRAY2BGR ); // ��ʾģʽ
		if( image1->origin )
			track_box1.angle = -track_box1.angle;
		cvEllipseBox( image1, track_box1, CV_RGB(255,0,0), 3, CV_AA, 0 );//�������ٽ����λ��
	}
	cvShowImage( "CamShiftDemo1", image1 );//��ʾ��Ƶ��ֱ��ͼ
	//cvShowImage( "Histogram", histimg );
}