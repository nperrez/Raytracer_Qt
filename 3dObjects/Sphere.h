//
// Created by Nicolas Perrez on 26/02/2026.
//

#ifndef RAYTRACER_QT_BALL_H
#define RAYTRACER_QT_BALL_H
#include "../util/Hit.h"
#include "../util/Ray.h"
#include "../util/Vector3d.h"
#include <../../../util/Material.h>
#include "Object.h"


class Sphere : public Object {

private:
    Vector3d center;
    double radius;

public:
    Sphere(const Vector3d &center, double radius, const Material &material);

    [[nodiscard]] Vector3d getCenter() const;
    [[nodiscard]] double getRadius() const;
    [[nodiscard]] double getDepth() const;

    [[nodiscard]] Hit intersect(Ray ray) const override;
};


#endif //RAYTRACER_QT_BALL_H