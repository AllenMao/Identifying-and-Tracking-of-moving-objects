#include"SIFT.h"
 Sift:: Sift()
{
	cout<<"Press 'q' to starting SIFT��"<<endl;
	{
		Mat img_1=imread("L.jpg",CV_LOAD_IMAGE_GRAYSCALE);//�궨��ʱCV_LOAD_IMAGE_GRAYSCALE=0��Ҳ���Ƕ�ȡ�Ҷ�ͼ��
		Mat img_2=imread("R.jpg",CV_LOAD_IMAGE_GRAYSCALE);//һ��Ҫ�ǵ�����·����б�߷�������Matlab�������෴��

		if(!img_1.data || !img_2.data)//�������Ϊ��
		{
			cout<<"opencv error"<<endl;
			return;
		}
		cout<<"open right"<<endl;

		//��һ������SIFT���Ӽ��ؼ���

		SiftFeatureDetector detector;//���캯�������ڲ�Ĭ�ϵ�
		std::vector<KeyPoint> keypoints_1,keypoints_2;//����2��ר���ɵ���ɵĵ����������洢������

		detector.detect(img_1,keypoints_1);//��img_1ͼ���м�⵽��������洢��������keypoints_1��
		detector.detect(img_2,keypoints_2);//ͬ��


		//��ͼ���л���������
		Mat img_keypoints_1,img_keypoints_2;

		drawKeypoints(img_1,keypoints_1,img_keypoints_1,Scalar::all(-1),DrawMatchesFlags::DEFAULT);//���ڴ��л���������
		drawKeypoints(img_2,keypoints_2,img_keypoints_2,Scalar::all(-1),DrawMatchesFlags::DEFAULT);

		imshow("sift_keypoints_1",img_keypoints_1);//��ʾ������
		imshow("sift_keypoints_2",img_keypoints_2);

		//������������
		SiftDescriptorExtractor extractor;//���������Ӷ���

		Mat descriptors_1,descriptors_2;//������������ľ���

		extractor.compute(img_1,keypoints_1,descriptors_1);//������������
		extractor.compute(img_2,keypoints_2,descriptors_2);

		//��burte force����ƥ����������
		BruteForceMatcher<L2<float>>matcher;//����һ��burte force matcher����
		vector<DMatch>matches;
		matcher.match(descriptors_1,descriptors_2,matches);

		/*FLANN����ƥ���㷨*/
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
		//����ƥ���߶�
		Mat img_matches;
		drawMatches(img_1,keypoints_1,img_2,keypoints_2,good_matches,img_matches);//��ƥ������Ľ�������ڴ�img_matches��

		//��ʾƥ���߶�
		imshow("sift_Matches.txt",img_matches);//��ʾ�ı���ΪMatches
		//cout<<good_matches.size()<<endl;
		//����ƥ��������
		FILE* fp = fopen("good_matches.txt", "wt");
		for(i =0;i<good_matches.size();i++ )
		{
			//good_matches[i].queryIdx�����ŵ�һ��ͼƬƥ������ţ�keypoints_1[good_matches[i].queryIdx].pt.xΪ����Ŷ�Ӧ�ĵ��x���ꡣy����ͬ��
			//good_matches[i].trainIdx�����ŵڶ���ͼƬƥ������ţ�keypoints_2[good_matches[i].trainIdx].pt.xΪΪ����Ŷ�Ӧ�ĵ��x���ꡣy����ͬ��
			
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