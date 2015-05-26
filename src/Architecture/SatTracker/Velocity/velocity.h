#ifndef VELOCITY_H
#define VELOCITY_H

#include <stdio.h>
#include <iostream>
#include <math.h>
#include <time.h>
#include <opencv2/ml/ml.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

#define MAX_ROWS 120                        //Number of rows in image (pixels)
#define MAX_COLS 120                        //Number of columns in image (pixels)
#define FOCAL_LENGTH 30.4                   //Focal length (milimeters)
#define H_SAT 100                           //Size of satellite (milimeters)
#define PX_TO_MM (FOCAL_LENGTH/MAX_ROWS)    //Convert pixels to milímetres (milimeters/pixels)

using namespace cv;

typedef struct CircleData {
    Point center;
    double radius;
} CircleData;

typedef struct Velocity {
    float velX;
    float velY;
    float velZ;
} Velocity;

static void drawCircle(Mat &imageData, CircleData &cercle);
static void rectToCircle(Rect r, CircleData &circle);
static void velocityXYZ(Velocity &vel, CircleData ini, CircleData fi, float sample_time);

#endif