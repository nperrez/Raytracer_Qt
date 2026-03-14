//
// Created by nicolas on 12.03.26.
//

#include "Triangle.h"

Triangle::Triangle(Vector3d a, Vector3d b, Vector3d c, Color color) : a(a), b(b), c(c), color(color) {
    normal = (b-a)/(c-a);
}

Triangle::Triangle(Vector3d a, Vector3d b, Vector3d c) : a(a), b(b), c(c), color(Color(0, 0, 0)) {
    normal = (b-a)/(c-a);
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

Color Triangle::getColor() const {
    return color;
}

double Triangle::getArea() const {
    return 0.5*normal.getLength();
}

Hit Triangle::intersect(Ray ray) const {
    double lambda = ((a-ray.getLocation())*normal)/(ray.getDirection()*normal);
    if (lambda > 0) {
        Hit hit = Hit(lambda, ray.getLocation() + ray.getDirection() * lambda, normal, color);
        Triangle pbc = Triangle(hit.getPosition(), b, c);
        Triangle pca = Triangle(hit.getPosition(), c, a);
        Triangle pab = Triangle(hit.getPosition(), a, b);
        double lambdaA = pbc.getArea()/this->getArea();
        double lambdaB = pca.getArea()/this->getArea();
        double lambdaC = pab.getArea()/this->getArea();
        if (lambdaA >= 0 && lambdaB >= 0 && lambdaC >= 0 && std::abs(lambdaA + lambdaB + lambdaC - 1.0) < 1e-6) {
            return {lambda, ray.getLocation() + ray.getDirection() * lambda, normal, color};
        }
        return {-1, ray.getLocation() + ray.getDirection() * lambda, normal, color};
    }
    return {-1, ray.getLocation() + ray.getDirection() * lambda, normal, color};
}
