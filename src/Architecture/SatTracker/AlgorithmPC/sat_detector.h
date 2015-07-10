#ifndef SAT_DETECTOR_H
#define SAT_DETECTOR_H

#include <math.h>
#include <ios>
#include <fstream>
#include <stdexcept>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/ml/ml.hpp>

using namespace std;
using namespace cv;

typedef struct CircleData {
    Point center;
    double radius;
} CircleData;

void configureHOGDetector(HOGDescriptor& hog, string& descriptorVectorFile);
void detectSat(HOGDescriptor& hog, const double hitThreshold, Mat& imageData, CircleData& cercle);

#endif