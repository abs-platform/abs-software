#include "velocity.h"

using namespace std;
using namespace cv;

static void drawCircle(Mat &imageData, CircleData &cercle)
{
    Scalar color = Scalar(0,255,0);
    circle(imageData, cercle.center, cercle.radius, color, 1, 8, 0); 

    //Show results  
    namedWindow("Detection", MAX_ROWS);
    imshow("Detection", imageData);
}

static void rectToCircle(Rect r, CircleData &circle)
{
    int a, b;
    a = round(r.width/2);
    b = round(r.height/2);
    circle.center = Point(r.x + a, r.y + b);
    circle.radius = hypotf(a,b);
}


int main(int argc, char *argv[])
{
    Mat imageData = imread("background.png", CV_LOAD_IMAGE_COLOR);
    Rect rectangle1 = Rect(MAX_ROWS/4, MAX_COLS/4, 30, 30);
    CircleData c1;
    
    rectToCircle(rectangle1, c1);
    drawCircle(imageData, c1); 
    waitKey(0);
    
    imwrite("result.png",imageData);

    return 0;
}
