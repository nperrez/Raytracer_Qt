//
// Created by Nicolas Perrez on 26/02/2026.
//

#include "Ray.h"

Ray::Ray(int x, int y) {
    this->location = Vector3d(x, y, 0);
    this->direction = Vector3d(0,0,1);
}

Ray::Ray(Vector3d location, Vector3d direction) : location(location), direction(direction) {}

Vector3d Ray::getLocation() {
    return this->location;
}

const Vector3d Ray::getDirection() {
    return this->direction;
}

bool Ray::isDone() {
    return done;
}

void Ray::resetY() {
    location = Vector3d(this->location.getX(), 0, 0);
}

void Ray::incrementX() {
    location = location + Vector3d(1,0,0);
}

void Ray::incrementY() {
    location = location + Vector3d(0,1,0);
}