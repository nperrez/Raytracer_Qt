//
// Created by Nicolas Perrez on 26/02/2026.
//

#ifndef RAYTRACER_QT_CIRCLEVECT_H
#define RAYTRACER_QT_CIRCLEVECT_H
#include "Color.h"
#include "Vector2d.h"


class CircleVect {

private:
    Vector2d center;
    int radius;
    Color color;

public:
    CircleVect(const Vector2d& center, const int radius, Color color);
    Vector2d getCenter() const;
    int getRadius() const;
    Color getColor() const;
};


#endif RAYTRACER_QT_CIRCLEVECT_H

