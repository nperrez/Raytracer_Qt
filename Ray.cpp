//
// Created by Nicolas Perrez on 26/02/2026.
//

#include "Ray.h"

Ray::Ray(int x, int y) {
    this->location = Vector3d(x, y, 0);
    this->direction = Vector3d(0,0,1);
}

Vector3d Ray::getLocation() {
    return this->location;
}

Vector3d Ray::getDirection() {
    return this->direction;
}

bool Ray::isDone() {
    return done;
}

void Ray::increment() {
    if (location.getX() < width) {
        location = location+Vector3d(1,0,0);
    } else if (location.getY() < height) {
        location = location+Vector3d(0,1,0);
    } else {
        done = true;
    }
}