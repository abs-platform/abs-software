#include "sat_tracker.h"

static string descriptorVectorFile = "descriptorvector.dat";

int main(int argc, char** argv)
{
    const float sample_time = 2.0;    //Sample time in miliseconds
    Mat image_initial, image_final;
    readSamplesFromFile(image_initial, image_final);
    
    HOGDescriptor hog;
    configureHOGDetector(hog, descriptorVectorFile);
    const double hitThreshold = 0.1f;

    CircleData cercle_initial;
    CircleData cercle_final;
    detectSat(hog, hitThreshold, image_initial, cercle_initial);
    detectSat(hog, hitThreshold, image_final, cercle_final);

    Velocity velocity;
    velocityXYZ(velocity, cercle_initial, cercle_final, sample_time);
    printVelocity(velocity);

    //Save results
    imwrite("carpeta_test/result_1.png", image_initial);
	imwrite("carpeta_test/result_2.png", image_final);
	cout << "Results saved in the results folder!\n";

    return 0;
}