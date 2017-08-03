class FrameDifference
{
public:
	FrameDifference(bool bReadFromFile, char *fileName, char *fileName1);
	~FrameDifference(){};

	CvScalar hsv2rgb( float hue );
	int MeanShift(IplImage* frame);
	int MeanShift1(IplImage* frame);

private:
	int minX,minY,maxX,maxY;
	/*meanshift-begin*/
	IplImage *image, *hsv, *hue, *mask, *backproject, *histimg;//��HSV�е�Hue�������и���
	CvHistogram *hist;//ֱ��ͼ��

	int backproject_mode;//��ʾ�Ƿ�Ҫ���뷴��ͶӰģʽ��ture��ʾ׼�����뷴��ͶӰģʽ
	int select_object;//�����Ƿ���ѡҪ���ٵĳ�ʼĿ�꣬true��ʾ���������ѡ��
	int track_object;//�������Ŀ����Ŀ
	int show_hist;//�Ƿ���ʾֱ��ͼ
	CvPoint origin;//���ڱ������ѡ���һ�ε���ʱ���λ��
	CvRect selection;//���ڱ������ѡ��ľ��ο�
	CvRect track_window;
	CvBox2D track_box; // Meanshift�����㷨���ص�Box��
	CvConnectedComp track_comp;

	int hdims; // ����ֱ��ͼbins�ĸ�����Խ��Խ��ȷ
	int NumFrames;
	int vmin, vmax, smin;
	/*meanshift-end*/

	int minX1,minY1,maxX1,maxY1;
	/*meanshift-begin*/
	IplImage *image1, *hsv1, *hue1, *mask1, *backproject1, *histimg1;//��HSV�е�Hue�������и���
	CvHistogram *hist1;//ֱ��ͼ��

	int backproject_mode1;//��ʾ�Ƿ�Ҫ���뷴��ͶӰģʽ��ture��ʾ׼�����뷴��ͶӰģʽ
	int select_object1;//�����Ƿ���ѡҪ���ٵĳ�ʼĿ�꣬true��ʾ���������ѡ��
	int track_object1;//�������Ŀ����Ŀ
	int show_hist1;//�Ƿ���ʾֱ��ͼ
	CvPoint origin1;//���ڱ������ѡ���һ�ε���ʱ���λ��
	CvRect selection1;//���ڱ������ѡ��ľ��ο�
	CvRect track_window1;
	CvBox2D track_box1; // Meanshift�����㷨���ص�Box��
	CvConnectedComp track_comp1;

	int hdims1; // ����ֱ��ͼbins�ĸ�����Խ��Խ��ȷ
	int NumFrames1;
	int vmin1, vmax1, smin1;
	/*meanshift-end*/
};