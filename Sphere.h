//
// Created by Nicolas Perrez on 26/02/2026.
//

#ifndef RAYTRACER_QT_BALL_H
#define RAYTRACER_QT_BALL_H
#include "Ray.h"
#include "Vector3d.h"


class Sphere {

private:
    Vector3d center;
    double radius;
    Ray ray;
    double a;

public:
    Sphere(Vector3d center, double radius, Ray ray);

    Vector3d get_center();
    double get_radius();

    double getLambda();
    Ray get_ray();
};


#endif //RAYTRACER_QT_BALL_H