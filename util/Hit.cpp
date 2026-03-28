//
// Created by nicolas on 05.03.26.
//

#include "Hit.h"

Hit::Hit(double lambda, Vector3d position, Vector3d normal, Material material) : material(&material), lambda(lambda), position(position), normal(normal) {}

double Hit::getLambda() {
    return lambda;
}

Vector3d Hit::getPosition() {
    return position;
}

Vector3d Hit::getNormal() {
    return normal;
}

std::shared_ptr<Material> Hit::getMaterial() {
    return material;
}

Color Hit::getColor() {
    return material->getAlbedo();
}

void Hit::setU(const double u) {
    this->u = u;
}

void Hit::setV(double v) {
    this->v = v;
}

double Hit::getU() {
    return u;
}

double Hit::getV() {
    return v;
}