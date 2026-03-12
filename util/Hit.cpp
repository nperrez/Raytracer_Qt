//
// Created by nicolas on 05.03.26.
//

#include "Hit.h"

Hit::Hit(double lambda, Vector3d position, Vector3d normal, Color color) : color(color), lambda(lambda), position(position), normal(normal) {}

double Hit::getLambda() {
    return lambda;
}

Vector3d Hit::getPosition() {
    return position;
}

Vector3d Hit::getNormal() {
    return normal;
}

Color Hit::getColor() {
    return color;
}

void Hit::setU(double u) {
    u = u;
}

void Hit::setV(double v) {
    v = v;
}

double Hit::getU() {
    return u;
}

double Hit::getV() {
    return v;
}