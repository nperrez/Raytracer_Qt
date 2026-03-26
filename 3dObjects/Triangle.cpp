//
// Created by nicolas on 12.03.26.
//

#include "Triangle.h"

Triangle::Triangle(const Vector3d &a, const Vector3d &b, const Vector3d &c, const Material &material) : a(a), b(b), c(c), material(material) {
    cross = (b-a)/(c-a);
    normal = cross/cross.getLength();
}

Triangle::Triangle(const Vector3d &a, const Vector3d &b, const Vector3d &c) : a(a), b(b), c(c), material(Lambert(Color(0, 0, 0))) {
    cross = (b-a)/(c-a);
    normal = cross/cross.getLength();
}

Vector3d Triangle::getA() const {
    return a;
}

Vector3d Triangle::getB() const {
    return b;
}

Vector3d Triangle::getC() const {
    return c;
}

Vector3d Triangle::getNormal() const {
    return normal;
}

Material Triangle::getMaterial() const {
    return material;
}

Color Triangle::getColor() const {
    return material.getAlbedo();
}

double Triangle::getArea() const {
    return 0.5*cross.getLength();
}

Hit Triangle::intersect(Ray ray) const {
    double lambda = ((a-ray.getLocation())*normal)/(ray.getDirection()*normal);
    if (lambda > 0) {
        auto hit = Hit(lambda, ray.getLocation() + ray.getDirection() * lambda, normal, this->getColor());
        auto pbc = Triangle(hit.getPosition(), b, c);
        auto pca = Triangle(hit.getPosition(), c, a);
        auto pab = Triangle(hit.getPosition(), a, b);
        double lambdaA = pbc.getArea()/this->getArea();
        double lambdaB = pca.getArea()/this->getArea();
        double lambdaC = pab.getArea()/this->getArea();
        if (lambdaA >= 0 && lambdaB >= 0 && lambdaC >= 0 && std::abs(lambdaA + lambdaB + lambdaC - 1.0) < 1e-6) {
            return {lambda, ray.getLocation() + ray.getDirection() * lambda, normal, this->getColor()};
        }
        return {-1, ray.getLocation() + ray.getDirection() * lambda, normal, this->getColor()};
    }
    return {-1, ray.getLocation() + ray.getDirection() * lambda, normal, this->getColor()};
}
