#ifndef SAT_VELOCITY_H
#define SAT_VELOCITY_H

#include "sat_detector.h"

#define MAX_ROWS 120                        //Number of rows in image (pixels)
#define MAX_COLS 120                        //Number of columns in image (pixels)
#define FOCAL_LENGTH 30.4                   //Focal length (milimeters)
#define H_SAT 100                           //Size of satellite (milimeters)
#define PX_TO_MM (FOCAL_LENGTH/MAX_COLS)    //Convert pixels to milímetres (milimeters/pixels)

typedef struct Velocity {
    float velX;
    float velY;
    float velZ;
} Velocity;

void velocityXYZ(Velocity &velocity, CircleData ini, CircleData fi, float sample_time);
void printVelocity(Velocity &velocity);

#endif