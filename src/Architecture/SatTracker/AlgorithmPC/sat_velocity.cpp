#include "sat_velocity.h"

void velocityXYZ(Velocity &velocity, CircleData ini, CircleData fi, float sample_time)
{
    velocity.velX = (fi.center.x - ini.center.x)/sample_time*PX_TO_MM;
    velocity.velY = (fi.center.y - ini.center.y)/sample_time*PX_TO_MM;
    velocity.velZ = (1-ini.radius/fi.radius)*H_SAT*FOCAL_LENGTH/(ini.radius*sample_time*PX_TO_MM);
}

void printVelocity(Velocity &velocity)
{
	printf("Velocity in X: %.2f m/s\n", velocity.velX);
    printf("Velocity in Y: %.2f m/s\n", velocity.velY);
    printf("Velocity in Z: %.2f m/s\n", velocity.velZ);
}