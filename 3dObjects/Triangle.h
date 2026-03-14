//
// Created by nicolas on 12.03.26.
//

#ifndef RAYTRACER_QT_TRIANGLE_H
#define RAYTRACER_QT_TRIANGLE_H
#include <../../../util/Vector3d.h>
#include <../../../util/Color.h>
#include <../../../util/Ray.h>
#include <../../../util/Hit.h>

class Triangle {

private:
    Vector3d a, b, c, normal;
    Color color;

public:
    Triangle(Vector3d a, Vector3d b, Vector3d c, Color color);
    Triangle(Vector3d a, Vector3d b, Vector3d c);
    [[nodiscard]] Vector3d getA() const;
    [[nodiscard]] Vector3d getB() const;
    [[nodiscard]] Vector3d getC() const;
    [[nodiscard]] Vector3d getNormal() const;
    [[nodiscard]] Color getColor() const;
    [[nodiscard]] double getArea() const;
    [[nodiscard]] Hit intersect(Ray ray) const;

};


#endif //RAYTRACER_QT_TRIANGLE_H