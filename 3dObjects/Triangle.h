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
    Vector3d a, b, c, v, w;
    Color color;

public:
    Triangle(Vector3d a, Vector3d b, Vector3d c, Color color);
    Triangle(Vector3d a, Vector3d b, Vector3d c);
    Vector3d getA();
    Vector3d getB();
    Vector3d getC();
    Color getColor();
    Hit intersect(Ray ray) const;

};


#endif //RAYTRACER_QT_TRIANGLE_H