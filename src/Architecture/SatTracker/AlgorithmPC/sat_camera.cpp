#include "sat_camera.h"

void readSamplesFromFile(Mat& image_initial, Mat& image_final)
{
    image_initial = imread("carpeta_test/sat_1.png");
    if (!image_initial.data) {
		printf("Couldn't open initial image \n");
    }

    image_final = imread("carpeta_test/sat_2.png");
    if (!image_final.data) {
    	printf("Couldn't open final image \n");
    }
}