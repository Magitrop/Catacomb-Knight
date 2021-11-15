#include <iostream>
#include <math.h>
#include "Tilemap.h"

double MoveTowards(double current, double target, double maxSpeed)
{
    if (fabs(target - current) <= maxSpeed)
        return target;
    return current + (target - current > 0 ? 1 : -1) * maxSpeed;
}

double MoveTowards(double current, double target, double maxSpeed, bool *goal)
{
    if (fabs(target - current) <= maxSpeed)
    {
        *goal = true;
        return target;
    }
    *goal = false;
    return current + (target - current > 0 ? 1 : -1) * maxSpeed;
}

// returns true if target can be reached in a single step
bool CheckDistanceEquality(double from, double to, double maxSpeed)
{
    if (fabs(to - from) <= maxSpeed)
        return true;
    else return false;
}

// causes strange in-tile sprite offsets
// not for use
void MoveTowardsVector(double *currentA, double targetA, double *currentB, double targetB, double maxSpeed)
{
    double toVector_x = targetA - *currentA;
    double toVector_y = targetB - *currentB;

    double sqdist = toVector_x * toVector_x + toVector_y * toVector_y;
    if (sqdist == 0 || (maxSpeed >= 0 && sqdist <= maxSpeed * maxSpeed))
        return;

    double dist = sqrt(sqdist);
    *currentA += toVector_x / dist * maxSpeed;
    *currentB += toVector_y / dist * maxSpeed;
}