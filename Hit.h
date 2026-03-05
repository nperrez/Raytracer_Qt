//
// Created by nicolas on 05.03.26.
//

#ifndef RAYTRACER_QT_HIT_H
#define RAYTRACER_QT_HIT_H
#include "Color.h"
#include "Vector3d.h"


class Hit {

private:
    double lambda;
    Vector3d position;
    Vector3d normal;
    Color color;

public:
    Hit(double lambda, Vector3d position, Vector3d normal, Color color);
    double getLambda();
    Vector3d getPosition();
    Vector3d getNormal();
    Color getColor();
};


#endif //RAYTRACER_QT_HIT_H