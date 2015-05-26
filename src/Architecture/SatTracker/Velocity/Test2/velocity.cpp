#include "velocity.h"

using namespace std;
using namespace cv;

static void velocityXYZ(Velocity &vel, CircleData ini, CircleData fi, float sample_time)
{
    vel.velX = (fi.center.x - ini.center.x)/sample_time*PX_TO_MM;
    vel.velY = (fi.center.y - ini.center.y)/sample_time*PX_TO_MM;
    vel.velZ = (1-ini.radius/fi.radius)*H_SAT*FOCAL_LENGTH/(ini.radius*sample_time*PX_TO_MM);
}


int main(int argc, char *argv[])
{
    float sample_time = 2.0;        //Sample time in miliseconds

    CircleData c1;
    c1.center = Point(60,60);
    c1.radius = 20;

    CircleData c2;
    c2.center = Point(70,40);
    c2.radius = 120;

    Velocity velocity;
    velocityXYZ(velocity, c1, c2, sample_time);

    printf("Velocity in X: %.2f m/s\n", velocity.velX);
    printf("Velocity in Y: %.2f m/s\n", velocity.velY);
    printf("Velocity in Z: %.2f m/s\n", velocity.velZ);

    return 0;
}
