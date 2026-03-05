//
// Created by Nicolas Perrez on 26/02/2026.
//

#ifndef RAYTRACER_QT_RAY_H
#define RAYTRACER_QT_RAY_H
#include "Vector3d.h"


class Ray {

private:
    Vector3d location;
    Vector3d direction;
    int x, y;
    bool done =false;

public:
    Ray(int x, int y);
    Vector3d getLocation();
    Vector3d getDirection();
    bool isDone();
    void resetY();

    void incrementX();
    void incrementY();
};


#endif //RAYTRACER_QT_RAY_H