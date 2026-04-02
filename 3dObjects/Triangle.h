//
// Created by nicolas on 12.03.26.
//

#ifndef RAYTRACER_QT_TRIANGLE_H
#define RAYTRACER_QT_TRIANGLE_H
#include <../../../util/Vector3d.h>
#include <../../../util/Ray.h>
#include <../../../util/Hit.h>
#include <../../../util/Material.h>
#include "Object.h"

class Triangle : Object {

private:
    Vector3d a, b, c, cross, normal;

public:
    Triangle(const Vector3d &a, const Vector3d &b, const Vector3d &c, const Material &material);
    Triangle(const Vector3d &a, const Vector3d &b, const Vector3d &c);
    [[nodiscard]] Vector3d getA() const;
    [[nodiscard]] Vector3d getB() const;
    [[nodiscard]] Vector3d getC() const;
    [[nodiscard]] Vector3d getNormal() const;
    [[nodiscard]] double getArea() const;
    [[nodiscard]] Hit intersect(Ray ray) const;

};


#endif //RAYTRACER_QT_TRIANGLE_H