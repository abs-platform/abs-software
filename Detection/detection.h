#ifndef DETECTION_H
#define DETECTION_H

#include <stdio.h>
#include <dirent.h>
#include <ios>
#include <fstream>
#include <stdexcept>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/ml/ml.hpp>
#include "svmlight/svmlight.h"

#define SVM_MODEL SVMlight

using namespace cv;

static string toLowerCase(const string &in);
static void getFilesInDirectory(const string &dirName, vector<string> &fileNames, \
                                const vector<string> &validExtensions);
static void showDetections(const vector<Rect> &found, Mat &imageData);
static void detectSat(const HOGDescriptor &hog, const double hitThreshold, Mat &imageData);
static void getDescriptorVectorFromFile(string& filename, vector<float>& descriptorVector);

#endif
