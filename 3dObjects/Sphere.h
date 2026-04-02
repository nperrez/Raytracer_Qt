//
// Created by Nicolas Perrez on 26/02/2026.
//

#ifndef RAYTRACER_QT_BALL_H
#define RAYTRACER_QT_BALL_H
#include "../util/Color.h"
#include "../util/Hit.h"
#include "../util/Ray.h"
#include "../util/Vector3d.h"
#include <../../../util/Material.h>
#include "Object.h"


class Sphere : Object {

private:
    Vector3d center;
    double radius;

public:
    Sphere(const Vector3d &center, double radius, const Material &material);

    Vector3d getCenter() const;
    double getRadius() const;
    double getDepth() const;

    Hit intersect(Ray ray) const;
};


#endif //RAYTRACER_QT_BALL_H