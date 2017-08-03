#pragma once
#include "Main.h"
#include"Calibration.cpp"

using namespace std;

int main()
{
    bool g_bReadFromFile = true;
	Calibration(g_bReadFromFile);//双目标定

	//FrameDifference frameDifference(g_bReadFromFile, "R3.avi", "L3.avi");
	

	//StereoMatching stereomatching("im0.ppm","im1.ppm");//立体匹配
	

	cvWaitKey(0);//先点主窗口再点，在点其他窗口按任意键 正常退出？？？？？？？？？？
	return 0;
}
//madebao
