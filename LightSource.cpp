//
// Created by nicolas on 05.03.26.
//

#include "LightSource.h"

LightSource::LightSource(Vector3d position, Color color) : position(position), color(color) {}

Vector3d LightSource::getPosition() {
    return position;
}

Color LightSource::getColor() {
    return color;
}