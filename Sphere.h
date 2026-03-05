//
// Created by Nicolas Perrez on 26/02/2026.
//

#ifndef RAYTRACER_QT_BALL_H
#define RAYTRACER_QT_BALL_H
#include "Color.h"
#include "Hit.h"
#include "Ray.h"
#include "Vector3d.h"


class Sphere {

private:
    Vector3d center;
    double radius;
    Color color;

public:
    Sphere(const Vector3d &center, double radius, const Color &color);

    Vector3d getCenter() const;
    double getRadius() const;
    Color getColor() const;
    double getDepth() const;

    Hit intersect(Ray ray) const;
};


#endif //RAYTRACER_QT_BALL_H