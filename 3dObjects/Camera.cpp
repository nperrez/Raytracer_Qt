//
// Created by nicolas on 19.03.26.
//

#include "Camera.h"
#include <cmath>


Camera::Camera(Vector3d position, Vector3d lookAt, double fov, int width, int height) : width(width), height(height) {
    this->position = position;
    this->fov = fov;
    aspectRatio = static_cast<double>(width) / static_cast<double>(height);
    forward = (lookAt - position).normalize();
    right = (Vector3d(0, 1, 0)/forward).normalize();
    up = (right/forward).normalize();
    halfHeight = tan(fov*0.5);
    halfWidth = halfHeight * aspectRatio;
}

Vector3d Camera::getPosition() const {
    return position;
}

Vector3d Camera::getForward() const {
    return forward;
}

Vector3d Camera::getRight() const {
    return right;
}

Vector3d Camera::getUp() const {
    return up;
}

double Camera::getFOV() const {
    return fov;
}

double Camera::getAspectRatio() const {
    return aspectRatio;
}

double Camera::getHalfHeight() const {
    return halfHeight;
}

double Camera::getHalfWidth() const {
    return halfWidth;
}

Ray Camera::getRay(double x, double y) const {
    double u = 2*(x + 0.5)/width - 1;
    double v = 1 - 2*(y + 0.5)/height;
    Vector3d dir = forward + right * halfWidth * u + up * halfHeight * v;
    return Ray(position, dir.normalize());
}