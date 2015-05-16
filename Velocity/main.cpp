#include <stdio.h>
#include <iostream>
#include <math.h>
#include <time.h>
#include <opencv2/ml/ml.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

#define MAX_ROWS 120						//Number of rows in image (pixels)
#define MAX_COLS 120						//Number of columns in image (pixels)
#define FOCAL_LENGTH 30.4					//Focal length (milimeters)
#define H_SAT 100							//Size of satellite (milimeters)
#define PX_TO_MM (FOCAL_LENGTH/MAX_ROWS)	//Convert pixels to milímetres (milimeters/pixels)

using namespace std;
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

static void drawCircle(Mat &imageData, CircleData &cercle)
{
	Scalar color = Scalar(0,255,0);
	circle(imageData, cercle.center, cercle.radius, color, 1, 8, 0); 

	//Show results  
	//namedWindow("Detection", MAX_ROWS);
	//imshow("Detection", imageData);
}

static void rectToCircle(Rect r, CircleData &circle)
{
	int a, b;
	a = round(r.width/2);
	b = round(r.height/2);
	circle.center = Point(r.x + a, r.y + b);
	circle.radius = hypotf(a,b);
}

static void velocityXYZ(Velocity &vel, CircleData ini, CircleData fi, float sample_time) 
{
	vel.velX = (fi.center.x - ini.center.x)/sample_time*PX_TO_MM;
	vel.velY = (fi.center.y - ini.center.y)/sample_time*PX_TO_MM;
	vel.velZ = (1-ini.radius/fi.radius)*H_SAT*FOCAL_LENGTH/(ini.radius*sample_time*PX_TO_MM);
}


int main(int argc, char *argv[]) 
{
	double time_elapsed;
	float sample_time = 2.0;	//Sample time in miliseconds

	Rect rectangle1 = Rect(MAX_ROWS/4, MAX_COLS/4, 30, 30);
	Rect rectangle2 = Rect(MAX_ROWS/2, MAX_COLS/4, 40, 40);
	
  	CircleData c1;
	CircleData c2;
	rectToCircle(rectangle1, c1);
	rectToCircle(rectangle2, c2);

	Velocity velocity;
	clock_t begin = clock();
	velocityXYZ(velocity, c1, c2, sample_time);
  	clock_t end = clock();
	
	time_elapsed = double(end - begin) / CLOCKS_PER_SEC;

	printf("Velocity in X: %.2f m/s\n", velocity.velX);
	printf("Velocity in Y: %.2f m/s\n", velocity.velY);
	printf("Velocity in Z: %.2f m/s\n", velocity.velZ);
	printf("Time elapsed to compute the velocity: %.1lf us\n", time_elapsed*1e6);

	return 0;	
}

