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
	IplImage *image, *hsv, *hue, *mask, *backproject, *histimg;//用HSV中的Hue分量进行跟踪
	CvHistogram *hist;//直方图类

	int backproject_mode;//表示是否要进入反向投影模式，ture表示准备进入反向投影模式
	int select_object;//代表是否在选要跟踪的初始目标，true表示正在用鼠标选择
	int track_object;//代表跟踪目标数目
	int show_hist;//是否显示直方图
	CvPoint origin;//用于保存鼠标选择第一次单击时点的位置
	CvRect selection;//用于保存鼠标选择的矩形框
	CvRect track_window;
	CvBox2D track_box; // Meanshift跟踪算法返回的Box类
	CvConnectedComp track_comp;

	int hdims; // 划分直方图bins的个数，越多越精确
	int NumFrames;
	int vmin, vmax, smin;
	/*meanshift-end*/

	int minX1,minY1,maxX1,maxY1;
	/*meanshift-begin*/
	IplImage *image1, *hsv1, *hue1, *mask1, *backproject1, *histimg1;//用HSV中的Hue分量进行跟踪
	CvHistogram *hist1;//直方图类

	int backproject_mode1;//表示是否要进入反向投影模式，ture表示准备进入反向投影模式
	int select_object1;//代表是否在选要跟踪的初始目标，true表示正在用鼠标选择
	int track_object1;//代表跟踪目标数目
	int show_hist1;//是否显示直方图
	CvPoint origin1;//用于保存鼠标选择第一次单击时点的位置
	CvRect selection1;//用于保存鼠标选择的矩形框
	CvRect track_window1;
	CvBox2D track_box1; // Meanshift跟踪算法返回的Box类
	CvConnectedComp track_comp1;

	int hdims1; // 划分直方图bins的个数，越多越精确
	int NumFrames1;
	int vmin1, vmax1, smin1;
	/*meanshift-end*/
};