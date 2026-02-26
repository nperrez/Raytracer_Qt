//
// Created by nicolas on 2/20/26.
//

#ifndef RAYTRACER_QT_VECTOR2D_H
#define RAYTRACER_QT_VECTOR2D_H

class Vector2d {

private:
    double x;
    double y;

public:

    Vector2d();
    Vector2d(double x, double y);

    double getX() const;
    double getY() const;

    Vector2d operator+(const Vector2d &v2) const;
    Vector2d operator-(const Vector2d& v2) const;
    Vector2d operator*(const double scalar) const;
    double length() const;

};

#endif //RAYTRACER_QT_VECTOR2D_H