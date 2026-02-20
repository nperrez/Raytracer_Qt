//
// Created by nicolas on 2/20/26.
//

#ifndef RAYTRACER_QT_VECTOR2D_H
#define RAYTRACER_QT_VECTOR2D_H


class Vector2d {

public:
    double x;
    double y;

    Vector2d();
    Vector2d(double x, double y);
    Vector2d add(Vector2d);
    Vector2d sub(Vector2d);
    Vector2d mul(double);
    double length();

};


#endif //RAYTRACER_QT_VECTOR2D_H