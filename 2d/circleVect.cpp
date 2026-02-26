//
// Created by Nicolas Perrez on 26/02/2026.
//

#include "circleVect.h"

CircleVect::CircleVect(const Vector2d &center, const int radius, Color color) : radius(radius), color(color) {
    this->center = center;
}

Vector2d CircleVect::getCenter() const {
    return this->center;
}

int CircleVect::getRadius() const {
    return this->radius;
}

Color CircleVect::getColor() const {
    return this->color;
}