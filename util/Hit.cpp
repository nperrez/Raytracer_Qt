//
// Created by nicolas on 05.03.26.
//

#include "Hit.h"

Hit::Hit(double lambda, Vector3d position, Vector3d normal, const Material &material) : material(material), lambda(lambda), position(position), normal(normal) {
    frontFace = true;
}

Hit::Hit() : material(Material(Color(0, 0, 0))) {
    lambda = 0;
    position = Vector3d(0, 0, 0);
    normal = Vector3d(0, 0, 0);
}

double Hit::getLambda() {
    return lambda;
}

Vector3d Hit::getPosition() {
    return position;
}

Vector3d Hit::getNormal() {
    return normal;
}

Material Hit::getMaterial() {
    return material;
}

Color Hit::getColor() {
    return material.getAlbedo();
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

void Hit::setFrontFace(const Vector3d& rayDirection, const Vector3d& outwardNormal) {
    frontFace = rayDirection * outwardNormal < 0;
    normal = frontFace ? outwardNormal : outwardNormal * -1;
}

bool Hit::isFrontFace() const {
    return frontFace;
}
