#pragma once
#include "Main.h"
#include"Calibration.cpp"

using namespace std;

int main()
{
    bool g_bReadFromFile = true;
	Calibration(g_bReadFromFile);//˫Ŀ�궨

	//FrameDifference frameDifference(g_bReadFromFile, "R3.avi", "L3.avi");
	

	//StereoMatching stereomatching("im0.ppm","im1.ppm");//����ƥ��
	

	cvWaitKey(0);//�ȵ��������ٵ㣬�ڵ��������ڰ������ �����˳���������������������
	return 0;
}
//madebao
