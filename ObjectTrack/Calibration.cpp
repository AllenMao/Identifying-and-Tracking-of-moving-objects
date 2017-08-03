#pragma once
#include "cv.h"
#include "highgui.h"
#include <stdio.h>
#include <iostream>
#include<vector>
#include<ctype.h>
using namespace std;
using namespace cv;


void Calibration(bool bReadFromFile)
{
	IplImage *leftimg,*rightimg,*image;
	CvCapture *leftcap=NULL,*rightcap=NULL;
	if(bReadFromFile)
	{
        leftcap=cvCaptureFromFile("1.avi");
        rightcap=cvCaptureFromFile("2.avi");
	}
	else
	{
        leftcap=cvCreateCameraCapture(1);
        rightcap=cvCreateCameraCapture(0);
	}

	image=cvQueryFrame(leftcap);
	image=cvQueryFrame(rightcap);
	if(leftcap==NULL||rightcap==NULL)
	{
		cout<<"Video or camera error!"<<endl;
		cvWaitKey(0);
		return;
	}

	///左右两个照片空间的申请
	leftimg=cvCloneImage(image);
	rightimg=cvCloneImage(image);

	cvNamedWindow("left",1);
	cvNamedWindow("right",1);
	int index=0,totalPic=10;//
	char *baseLeftName="jpgleft",*baseRightName="jpgright";
	char fileName[100];
	FILE* file=fopen("fileName.txt","wt");
	char *buf="7 7\n";//定义内角点规格
	fwrite(buf,1,strlen(buf),file);

	while(1)
	{
		image=cvQueryFrame(leftcap);
		if(!image) break;
		cvCopy(image,leftimg);
		image=cvQueryFrame(rightcap);
		if(!image) break;
		cvCopy(image,rightimg);

		cvShowImage("left",leftimg);
		cvShowImage("right",rightimg);
		char ch=cvWaitKey(1000);
		if(ch=='s')
		{
			cout<<"Collecting "<<index+1<<"th picture//"<<totalPic<<endl;
			char *temp="\n";

			sprintf_s(fileName,100,"%s%d.jpg",baseLeftName,index);
			cvSaveImage(fileName,leftimg);
			fwrite(fileName,1,strlen(fileName),file);//写入文件名
			fwrite(temp,1,strlen(temp),file);

			sprintf_s(fileName,100,"%s%d.jpg",baseRightName,index++);
			cvSaveImage(fileName,rightimg);
			fwrite(fileName,1,strlen(fileName),file);//写入文件名
			fwrite(temp,1,strlen(temp),file);

			if(index==totalPic) break;
		}
	}
	fclose(file);

	cvDestroyWindow("left");
	cvDestroyWindow("right");

	cout<<"Starting Calibration!"<<endl;
	void StereoCalib(const char *imageList);
	StereoCalib("fileName.txt");
}

// Given a list of chessboard images, the number of corners (nx, ny)
// on the chessboards, and a flag: useCalibrated for calibrated (0) or
// uncalibrated (1: use cvStereoCalibrate(), 2: compute fundamental
// matrix separately) stereo. Calibrate the cameras and display the
// rectified results along with the computed disparity images.
void StereoCalib(const char *imageList)
{
	int showUndistorted=1,displayCorners=1;//是否显示标记角点
	int nx,ny;
	const float squareSize = 1.f;

	vector<string> imageNames[2];
	vector<CvPoint3D32f> objectPoints;
	vector<CvPoint2D32f> points[2];
	vector<CvPoint2D32f> temp_points[2];
	vector<CvPoint2D32f> temp;
	vector<int> npoints;

	// ARRAY AND VECTOR STORAGE:
	double M1[3][3], M2[3][3], D1[5], D2[5];
	double R[3][3], T[3], E[3][3], F[3][3];
	double Q[4][4];
	CvMat _M1 = cvMat(3, 3, CV_64F, M1 );
	CvMat _M2 = cvMat(3, 3, CV_64F, M2 );
	CvMat _D1 = cvMat(1, 5, CV_64F, D1 );
	CvMat _D2 = cvMat(1, 5, CV_64F, D2 );
	CvMat matR = cvMat(3, 3, CV_64F, R );
	CvMat matT = cvMat(3, 1, CV_64F, T );
	CvMat matE = cvMat(3, 3, CV_64F, E );
	CvMat matF = cvMat(3, 3, CV_64F, F );

	CvMat matQ = cvMat(4, 4, CV_64FC1, Q);


	int is_found[2],nframes=0;
	CvSize imageSize = {0,0};

	cvNamedWindow( "corners", 1 );

	FILE* file = fopen(imageList, "rt");
	// READ IN THE LIST OF CHESSBOARDS:
	if( !file)
	{
		fprintf(stderr, "can not open file %s\n", imageList );
		return;
	}

	char buf[1000];
	if( !fgets(buf, sizeof(buf)-3, file) ||sscanf(buf, "%d%d", &nx, &ny) != 2 )
		return;

	int n = nx*ny;
	temp.resize(n);
	temp_points[0].resize(n);
	temp_points[1].resize(n);

	for(int i=0;;i++)
	{
		int count=0,result=0;
		int lr = i % 2;
		vector<CvPoint2D32f>& pts = temp_points[lr];//temp_points

		if( !fgets( buf, sizeof(buf)-3, file)) break;
		size_t len = strlen(buf);
		while( len > 0 && isspace(buf[len-1])) buf[--len] = '\0';//isspace检查空字符
		if( buf[0] == '#') continue;

		IplImage* img = cvLoadImage(buf,0);
		if( !img ) break;

		imageSize=cvGetSize(img);
		imageNames[lr].push_back(buf);
		//FIND CHESSBOARDS AND CORNERS THEREIN:
		result = cvFindChessboardCorners(img, cvSize(nx, ny),&temp[0], &count,
				CV_CALIB_CB_ADAPTIVE_THRESH |CV_CALIB_CB_NORMALIZE_IMAGE);

		if(displayCorners)
		{
			printf("%s\n", buf);
			IplImage* cimg = cvCreateImage( imageSize, 8, 3 );
			cvCvtColor( img, cimg, CV_GRAY2BGR );
			cvDrawChessboardCorners( cimg, cvSize(nx, ny), &temp[0],count, result );
			IplImage* cimg1 = cvCreateImage(cvSize(640, 480), IPL_DEPTH_8U, 3);
			cvResize(cimg, cimg1);
			cvShowImage( "corners", cimg1 );
			cvReleaseImage( &cimg );
			cvReleaseImage( &cimg1 );
		}

		is_found[lr] =result > 0 ? 1 : 0;
		if( result )
		{
			//Calibration will suffer without subpixel interpolation 提高精度
			cvFindCornerSubPix( img, &temp[0], count,cvSize(11, 11), cvSize(-1,-1),
				cvTermCriteria(CV_TERMCRIT_ITER+CV_TERMCRIT_EPS,30, 0.01) );
			copy( temp.begin(), temp.end(), pts.begin() );
		}
		cvReleaseImage( &img );

		if(lr)
		{
			if(is_found[0] == 1 && is_found[1] == 1)
			{
				assert(temp_points[0].size() == temp_points[1].size());//断言，假设相等
				int current_size = points[0].size();

				points[0].resize(current_size+temp_points[0].size(),cvPoint2D32f(0.0, 0.0));
				points[1].resize(current_size+temp_points[1].size(),cvPoint2D32f(0.0, 0.0));

				copy(temp_points[0].begin(), temp_points[0].end(), points[0].begin() + current_size);
				copy(temp_points[1].begin(), temp_points[1].end(), points[1].begin() + current_size);

				nframes++;

				printf("Pair successfully detected...\n");
			}
			is_found[0] = 0;
			is_found[1] = 0;
		}
	}
	fclose(file);
	printf("\n");

	// HARVEST CHESSBOARD 3D OBJECT POINT LIST:
	objectPoints.resize(nframes*n);
	for(int  i = 0; i < ny; i++ )
		for(int j = 0; j < nx; j++ )
			objectPoints[i*nx + j]=cvPoint3D32f(i*squareSize, j*squareSize, 0);
	for(int i = 1; i < nframes; i++ )
		copy( objectPoints.begin(), objectPoints.begin() + n,objectPoints.begin() + i*n );
	npoints.resize(nframes,n);
	int N = nframes*n;

	CvMat _objectPoints = cvMat(1, N, CV_32FC3, &objectPoints[0] );
	CvMat _imagePoints1 = cvMat(1, N, CV_32FC2, &points[0][0] );
	CvMat _imagePoints2 = cvMat(1, N, CV_32FC2, &points[1][0] );
	CvMat _npoints = cvMat(1, npoints.size(), CV_32S, &npoints[0] );
	cvSetIdentity(&_M1);//cvSetIdentity()将会把数组中除了行数与列数相等以外的所有元素的值都设置为0；行数与列数相等的元素的值都设置为1。甚至不要求数组的行数与列数相等。
	cvSetIdentity(&_M2);
	cvZero(&_D1);
	cvZero(&_D2);

	// CALIBRATE THE STEREO CAMERAS
	printf("Running stereo calibration ...");
	fflush(stdout);
	cvStereoCalibrate( &_objectPoints, &_imagePoints1,
		&_imagePoints2, &_npoints,
		&_M1, &_D1, &_M2, &_D2,
		imageSize, &matR, &matT, &matE, &matF,
		cvTermCriteria(CV_TERMCRIT_ITER+
		CV_TERMCRIT_EPS, 100, 1e-5),
		CV_CALIB_FIX_ASPECT_RATIO +
		CV_CALIB_ZERO_TANGENT_DIST +
		CV_CALIB_SAME_FOCAL_LENGTH +
		CV_CALIB_FIX_K3);
	printf(" done\n");
	// save intrinsic parameters
	CvFileStorage* fstorage = cvOpenFileStorage("intrinsics.yml", NULL, CV_STORAGE_WRITE);
	cvWrite(fstorage, "M1", &_M1);
	cvWrite(fstorage, "D1", &_D1);
	cvWrite(fstorage, "M2", &_M2);
	cvWrite(fstorage, "D2", &_D2);
	cvReleaseFileStorage(&fstorage);

	//COMPUTE AND DISPLAY RECTIFICATION
	if( showUndistorted )
	{
		CvMat* mx1 = cvCreateMat( imageSize.height,
			imageSize.width, CV_32F );
		CvMat* my1 = cvCreateMat( imageSize.height,
			imageSize.width, CV_32F );
		CvMat* mx2 = cvCreateMat( imageSize.height,
			imageSize.width, CV_32F );
		CvMat* my2 = cvCreateMat( imageSize.height,
			imageSize.width, CV_32F );
		CvMat* img1r = cvCreateMat( imageSize.height,
			imageSize.width, CV_8U );
		CvMat* img2r = cvCreateMat( imageSize.height,
			imageSize.width, CV_8U );
		CvMat* disp = cvCreateMat( imageSize.height,
			imageSize.width, CV_16S );

		double R1[3][3], R2[3][3], P1[3][4], P2[3][4];
		CvMat _R1 = cvMat(3, 3, CV_64F, R1);
		CvMat _R2 = cvMat(3, 3, CV_64F, R2);

		CvMat _P1 = cvMat(3, 4, CV_64F, P1);
		CvMat _P2 = cvMat(3, 4, CV_64F, P2);
		CvRect roi1, roi2;
		cvStereoRectify( &_M1, &_M2, &_D1, &_D2, imageSize,&matR, &matT,&_R1, &_R2, &_P1, &_P2, &matQ,
				CV_CALIB_ZERO_DISPARITY,1, imageSize, &roi1, &roi2);

		CvFileStorage* file = cvOpenFileStorage("extrinsics.yml", NULL, CV_STORAGE_WRITE);
		cvWrite(file, "R", &matR);
		cvWrite(file, "T", &matT);
		cvWrite(file, "R1", &_R1);
		cvWrite(file, "R2", &_R2);
		cvWrite(file, "P1", &_P1);
		cvWrite(file, "P2", &_P2);
		cvWrite(file, "Q", &matQ);
		cvReleaseFileStorage(&file);

		cvReleaseMat( &mx1 );
		cvReleaseMat( &my1 );
		cvReleaseMat( &mx2 );
		cvReleaseMat( &my2 );
		cvReleaseMat( &img1r );
		cvReleaseMat( &img2r );
		cvReleaseMat( &disp );
	}

	cout<<"标定已经完成，内参数保存在intrinsic.yml文件中，外参数保存在extrinsic.yml文件中!\n按任意键继续。"<<endl;

	cvWaitKey();
	cvDestroyWindow("corners");
}

int main()
{
	 Calibration(true);

	 cvWaitKey(0);
	 return 0;
}