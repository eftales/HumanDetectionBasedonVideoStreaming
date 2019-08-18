#ifndef __CameraHumanDetect__
#define __CameraHumanDetect__

#include <iostream>
#include <fstream>
#include <opencv2/opencv.hpp>
#include <cvaux.h>
#include <stdio.h>

#include "my_svm.h"

#define DEFAULT_CAMERA 1

#pragma warning(disable:4996)

cv::HOGDescriptor* GetmyHOG();

typedef struct my_struct_detect_result{
	int num;
	cv::Rect r;

}my_struct_detect_result;

#endif