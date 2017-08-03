#pragma once
#include"Main.h"
class StereoMatching
{
public:
	StereoMatching(char* img1_filename, char* img2_filename);
	~StereoMatching(){};
	void saveXYZ(const char* filename,Mat mat);
};
