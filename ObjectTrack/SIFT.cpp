#include"SIFT.h"
 Sift:: Sift()
{
	cout<<"Press 'q' to starting SIFT…"<<endl;
	{
		Mat img_1=imread("L.jpg",CV_LOAD_IMAGE_GRAYSCALE);//宏定义时CV_LOAD_IMAGE_GRAYSCALE=0，也就是读取灰度图像
		Mat img_2=imread("R.jpg",CV_LOAD_IMAGE_GRAYSCALE);//一定要记得这里路径的斜线方向，这与Matlab里面是相反的

		if(!img_1.data || !img_2.data)//如果数据为空
		{
			cout<<"opencv error"<<endl;
			return;
		}
		cout<<"open right"<<endl;

		//第一步，用SIFT算子检测关键点

		SiftFeatureDetector detector;//构造函数采用内部默认的
		std::vector<KeyPoint> keypoints_1,keypoints_2;//构造2个专门由点组成的点向量用来存储特征点

		detector.detect(img_1,keypoints_1);//将img_1图像中检测到的特征点存储起来放在keypoints_1中
		detector.detect(img_2,keypoints_2);//同理


		//在图像中画出特征点
		Mat img_keypoints_1,img_keypoints_2;

		drawKeypoints(img_1,keypoints_1,img_keypoints_1,Scalar::all(-1),DrawMatchesFlags::DEFAULT);//在内存中画出特征点
		drawKeypoints(img_2,keypoints_2,img_keypoints_2,Scalar::all(-1),DrawMatchesFlags::DEFAULT);

		imshow("sift_keypoints_1",img_keypoints_1);//显示特征点
		imshow("sift_keypoints_2",img_keypoints_2);

		//计算特征向量
		SiftDescriptorExtractor extractor;//定义描述子对象

		Mat descriptors_1,descriptors_2;//存放特征向量的矩阵

		extractor.compute(img_1,keypoints_1,descriptors_1);//计算特征向量
		extractor.compute(img_2,keypoints_2,descriptors_2);

		//用burte force进行匹配特征向量
		BruteForceMatcher<L2<float>>matcher;//定义一个burte force matcher对象
		vector<DMatch>matches;
		matcher.match(descriptors_1,descriptors_2,matches);

		/*FLANN特征匹配算法*/
		//-- Quick calculation of max and min distances between keypoints
		double max_dist=0;double min_dist=100;
		int i;
		for(i=0;i<descriptors_1.rows;++i)
		{
			double dist=matches[i].distance;
			if(dist<min_dist) min_dist=dist;
			if(dist>max_dist) max_dist=dist;
		}
		vector<DMatch>good_matches;
		for(i=0;i<descriptors_1.rows;++i)
		{
			if(matches[i].distance<3*min_dist)
			{
				good_matches.push_back( matches[i]);
			}
		}
		//绘制匹配线段
		Mat img_matches;
		drawMatches(img_1,keypoints_1,img_2,keypoints_2,good_matches,img_matches);//将匹配出来的结果放入内存img_matches中

		//显示匹配线段
		imshow("sift_Matches.txt",img_matches);//显示的标题为Matches
		//cout<<good_matches.size()<<endl;
		//保存匹配点的坐标
		FILE* fp = fopen("good_matches.txt", "wt");
		for(i =0;i<good_matches.size();i++ )
		{
			//good_matches[i].queryIdx保存着第一张图片匹配点的序号，keypoints_1[good_matches[i].queryIdx].pt.x为该序号对应的点的x坐标。y坐标同理
			//good_matches[i].trainIdx保存着第二张图片匹配点的序号，keypoints_2[good_matches[i].trainIdx].pt.x为为该序号对应的点的x坐标。y坐标同理
			
			fprintf(fp, "X1		Y1		X2		Y2\n");
			fprintf(fp, "%f		%f		%f		%f\n",keypoints_1[good_matches[i].queryIdx].pt.x,
				keypoints_1[good_matches[i].queryIdx].pt.y,
				keypoints_2[good_matches[i].trainIdx].pt.x,
				keypoints_2[good_matches[i].trainIdx].pt.y);
		}
		puts("hehe");
		fclose(fp);
	}
}