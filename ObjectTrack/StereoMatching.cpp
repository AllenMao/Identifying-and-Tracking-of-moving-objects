#include"StereoMatching.h"
#include"Main.h"
void StereoMatching::saveXYZ(const char* filename,Mat mat)
{
	//cout<<"max="<<mat.rows*mat.cols<<endl;
	const double max_z = 1.0e4;
	FILE* fp = fopen(filename, "wt");
	//fprintf(fp, "%d\n",mat.rows*mat.cols);
	//fprintf(fp, "\"X\"   \"Y\"   \"Z\"\n");
	
	for(int y = 0; y < mat.rows; y++)
	{
		for(int x = 0; x < mat.cols; x++)
		{
			Vec3f point = mat.at<Vec3f>(y, x);
			if(fabs(point[2] - max_z) < FLT_EPSILON || fabs(point[2]) > max_z) continue;
			if(point[2] > 300) continue;
			fprintf(fp, "%f\t%f\t%f\n", point[0], point[1], point[2]);
		}
	}
	fclose(fp);
}

//输入：jpgleft0.jpg jpgright0.jpg --algorithm=bm --blocksize=7 --max-disparity=96 -i intrinsics.yml -e extrinsics.yml -o depth.jpg -p point.xyz
StereoMatching::StereoMatching(char* img1_filename, char* img2_filename)
{
	//char* img1_filename =img1;
	//char* img2_filename =img2;
	const char* intrinsic_filename ="intrinsics.yml";
	const char* extrinsic_filename ="extrinsics.yml";
	const char* disparity_filename ="depth.jpg";
	const char* point_cloud_filename ="point.xyz";

	enum { STEREO_BM=0, STEREO_SGBM=1, STEREO_HH=2 };
	int alg = STEREO_SGBM;
	int SADWindowSize = 0, numberOfDisparities = 0;
	bool no_display = false;

	StereoBM bm;//计算两幅图的差异
	StereoSGBM sgbm;

	char* _alg ="bm";

	alg = strcmp(_alg, "bm") == 0 ? STEREO_BM :
		strcmp(_alg, "sgbm") == 0 ? STEREO_SGBM :strcmp(_alg, "hh") == 0 ? STEREO_HH : -1;

	numberOfDisparities=32;
	if(numberOfDisparities < 1 || numberOfDisparities % 16 != 0 )
	{
		printf("Command-line parameter error: The max disparity (--maxdisparity=<...>) must be a positive integer divisible by 16\n");
		return;
	}
	SADWindowSize=7;
	if(SADWindowSize < 1 || SADWindowSize % 2 != 1 )
	{
		printf("Command-line parameter error: The block size (--blocksize=<...>) must be a positive odd number\n");
		return;
	}
	//printf("%s %s %s %s\n",intrinsic_filename,extrinsic_filename,disparity_filename,point_cloud_filename);
	if( !img1_filename || !img2_filename )
	{
		printf("Command-line parameter error: both left and right images must be specified\n");
		return;
	}

	if( (intrinsic_filename != 0) ^ (extrinsic_filename != 0) )
	{
		printf("Command-line parameter error: either both intrinsic and extrinsic parameters must be specified, or none of them (when the stereo pair is already rectified)\n");
		return;
	}
	if( extrinsic_filename == 0 && point_cloud_filename )
	{
		printf("Command-line parameter error: extrinsic and intrinsic parameters must be specified to compute the point cloud\n");
		return;
	}

	IplImage *imge1=cvLoadImage(img1_filename);
	IplImage *imge2=cvLoadImage(img2_filename);
	cvShowImage( "CamShiftDemo1", imge1 );
	cvShowImage( "CamShiftDemo2", imge2 );
	////////////////////////////////////////////////////////
	int color_mode = alg == STEREO_BM ? 0 : -1;
	Mat img1 = imread(img1_filename, color_mode);//读取图片文件中的数据
	Mat img2 = imread(img2_filename, color_mode);
	////////////////////////////////////////////////
	//img1=imge1;
	//img2=imge2;

	CvSize img_size = cvGetSize (imge1);
	CvRect roi1, roi2;
	CvMat * Q = cvCreateMat (4, 4, CV_64FC1);
	if( intrinsic_filename )
	{
		// reading intrinsic parameters
		FileStorage fs(intrinsic_filename,CV_STORAGE_READ);
		if(!fs.isOpened())
		{
			printf("Failed to open file %s\n", intrinsic_filename);
			return;
		}

		Mat _M1, _D1, _M2, _D2;
		CvMat M1, D1, M2,D2;
		fs["M1"] >> _M1;
		fs["D1"] >> _D1;
		fs["M2"] >> _M2;
		fs["D2"] >> _D2;
		M1=_M1;
		M2=_M2;
		D1=_D1;
		D2=_D2;
		fs.open(extrinsic_filename, CV_STORAGE_READ);
		if(!fs.isOpened())
		{
			printf("Failed to open file %s\n", extrinsic_filename);
			return;
		}

		Mat _R, _T;
		CvMat R,T;
		CvMat *R1 = cvCreateMat (3,3,CV_64FC1);
		CvMat * R2 = cvCreateMat (3,3,CV_64FC1);
		CvMat * P1 = cvCreateMat (3,4,CV_64FC1);
		CvMat * P2 = cvCreateMat (3,4,CV_64FC1);

		fs["R"] >> _R;
		fs["T"] >> _T;
		R=_R;
		T=_T;

		cvStereoRectify( &M1, &M2, &D1, &D2, img_size, &R, &T, R1, R2, P1, P2, Q,
			CV_CALIB_ZERO_DISPARITY, 0, img_size, &roi1, &roi2);
		//cvStereoRectify( M1, D1, M2, D2, img_size, R, T, R1, R2, P1, P2, Q, 1,-1, img_size, &roi1, &roi2 );//对标定过的摄像机进行校正


		CvMat * map11 = cvCreateMat (img_size.height, img_size.width, CV_32FC1);
		CvMat * map12 = cvCreateMat (img_size.height, img_size.width, CV_32FC1);
		CvMat * map21 = cvCreateMat (img_size.height, img_size.width, CV_32FC1);
		CvMat * map22 = cvCreateMat (img_size.height, img_size.width, CV_32FC1);

		//CvMat map11, map12, map21, map22;
		cvInitUndistortRectifyMap (&M1,&D1, R1, P1, map11, map12);
		cvInitUndistortRectifyMap (&M2,&D2, R2, P2, map21, map22);
		//计算左右两幅图像的映射矩阵
		//initUndistortRectifyMap(M1, D1, R1, P1, img_size, CV_16SC2, map11, map12);
		//initUndistortRectifyMap(M2, D2, R2, P2, img_size, CV_16SC2, map21, map22);

		//CvMat img1r, img2r;
		//CvMat * img1r = cvCreateMat (img_size.height, img_size.width, CV_32FC1);
		//CvMat * img2r = cvCreateMat (img_size.height, img_size.width, CV_32FC1);
		IplImage * image1r = cvCreateImage(img_size,8,3); 
		IplImage * image2r = cvCreateImage(img_size,8,3); 
		// 把原始图像投影到新图像上，得到矫正图像
		cvRemap(imge1, image1r, map11, map12);
		cvRemap(imge2, image2r, map21, map22);

		//namedWindow("1", 1);
		//cvShowImage("1",image1r);

		//namedWindow("2", 1);
		//cvShowImage("2", image1r);
	}



	//计算三维坐标
	numberOfDisparities = numberOfDisparities > 0 ? numberOfDisparities : img_size.width/8;

	bm.state->roi1 = roi1;
	bm.state->roi2 = roi2;
	bm.state->preFilterCap = 31;
	bm.state->SADWindowSize = SADWindowSize > 0 ? SADWindowSize : 9;
	bm.state->minDisparity = 0;
	bm.state->numberOfDisparities = numberOfDisparities;
	bm.state->textureThreshold = 10;
	bm.state->uniquenessRatio = 15;
	bm.state->speckleWindowSize = 100;
	bm.state->speckleRange = 32;
	bm.state->disp12MaxDiff = 1;

	sgbm.preFilterCap = 63;
	sgbm.SADWindowSize = SADWindowSize > 0 ? SADWindowSize : 3;
	int cn = img1.channels();
	sgbm.P1 = 8*cn*sgbm.SADWindowSize*sgbm.SADWindowSize;
	sgbm.P2 = 32*cn*sgbm.SADWindowSize*sgbm.SADWindowSize;
	sgbm.minDisparity = 0;
	sgbm.numberOfDisparities = numberOfDisparities;
	sgbm.uniquenessRatio = 10;
	sgbm.speckleWindowSize = bm.state->speckleWindowSize;
	sgbm.speckleRange = bm.state->speckleRange;
	sgbm.disp12MaxDiff = 1;
	sgbm.fullDP = alg == STEREO_HH;

	Mat disp, disp8;
	//Mat img1p, img2p, dispp;
	//copyMakeBorder(img1, img1p, 0, 0, numberOfDisparities, 0, IPL_BORDER_REPLICATE);
	//copyMakeBorder(img2, img2p, 0, 0, numberOfDisparities, 0, IPL_BORDER_REPLICATE);
	;
	int64 t = getTickCount();
	//if( alg == STEREO_BM)
	bm(img1, img2, disp);
	//else
	//sgbm(img1, img2, disp);
	t = getTickCount() - t;
	printf("Time elapsed: %fms\n", t*1000/getTickFrequency());
	fflush(stdout);

	//disp = dispp.colRange(numberOfDisparities, img1p.cols);
	//converTo的方法来进行矩阵类型转换。当然使用时需要注意的是，该函数只能进行矩阵深度类型的转换，而不能进行矩阵通道数的转换
	disp.convertTo(disp8, CV_8U, 255/(numberOfDisparities*16.));
	if( !no_display )
	{
		//namedWindow("left", 1);
		//imshow("left", img1);

		//namedWindow("right", 1);
		//imshow("right", img2);

		//namedWindow("disparity", 0);
		//imshow("disparity", disp8);//视差图
		//printf("press any key to continue...");
		//fflush(stdout);
		//waitKey();
		//printf("\n");
	}


	if(disparity_filename)
	{
		imwrite(disparity_filename, disp8);
		//puts("hehe");
	}

	if(point_cloud_filename)
	{
		printf("storing the point cloud...");
		fflush(stdout);
		Mat xyz;
		//Mat::Mat(Q,false);

		reprojectImageTo3D(disp, xyz,Mat::Mat(Q,true), true);
		saveXYZ(point_cloud_filename, xyz);
		printf("\n");
	}
	printf("Success!!!\n");
	fflush(stdout);
}
