#ifndef SAT_CAMERA_H
#define SAT_CAMERA_H

#include <stdio.h>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>

using namespace cv;

void takePhoto(Mat& image_1, Mat& image_2);
void readSamplesFromFile(Mat& image_initial, Mat& image_final);

#endif