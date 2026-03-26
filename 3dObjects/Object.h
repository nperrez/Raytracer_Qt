//
// Created by nicolas on 26.03.26.
//

#ifndef RAYTRACER_QT_OBJECT_H
#define RAYTRACER_QT_OBJECT_H

#include <cmath>
#include <../../../util/Hit.h>
#include "../util/Ray.h"
#include "../util/Vector3d.h"
#include <../../../util/Material.h>

class Object {
public:
    Material material;
    Object(Material material) : material(material) {}
};

class Sphere : public Object {
private:
    Vector3d center;
    double radius;

public:
    Sphere(const Vector3d &center, double radius, const Material &material) : Object(material), center(center), radius(radius) {}

    Vector3d getCenter() const {
        return center;
    }
    double getRadius() const {
        return radius;
    }
    double getDepth() const {
        return center.getZ();
    }

    Hit intersect(Ray ray) const {
        const double a = ray.getDirection()*ray.getDirection();
        const Vector3d v = ray.getLocation() - center;
        const double b = 2*(ray.getDirection()*v);
        const double c = v*v - radius*radius;
        const double d = b*b - 4*a*c;
        if (d < 0) {
            return Hit(-1, Vector3d(0, 0, 0), Vector3d(0, 0, 0), Color(0, 0, 0));
        }

        double lambda1 = (-b-sqrt(d))/(2*a);

        if (lambda1>=0) {
            return Hit(lambda1, ray.getLocation()+ray.getDirection()*lambda1, (v+Vector3d(0, 0, lambda1))/(v+Vector3d(0, 0, lambda1)).getLength(), material.getAlbedo());
        }
        double lambda2 = (-b+sqrt(d))/(2*a);
        return Hit(lambda2, ray.getLocation()+ray.getDirection()*lambda2, (v+Vector3d(0, 0, lambda1))/(v+Vector3d(0, 0, lambda1)).getLength(), material.getAlbedo());
    }
};

class Triangle : public Object {
private:
    Vector3d a, b, c, cross, normal;

public:
    Triangle(const Vector3d &a, const Vector3d &b, const Vector3d &c, const Material &material) : Object(material), a(a), b(b), c(c) {
        cross = (b-a)/(c-a);
        normal = cross/cross.getLength();
    }
    Triangle(const Vector3d &a, const Vector3d &b, const Vector3d &c) : Object(Lambert(Color(0, 0, 0))), a(a), b(b), c(c) {
        cross = (b-a)/(c-a);
        normal = cross/cross.getLength();
    }
    [[nodiscard]] Vector3d getA() const {
        return a;
    }
    [[nodiscard]] Vector3d getB() const {
        return b;
    }
    [[nodiscard]] Vector3d getC() const {
        return c;
    }
    [[nodiscard]] Vector3d getNormal() const {
        return normal;
    }
    [[nodiscard]] double getArea() const {
        return 0.5*cross.getLength();
    }
    [[nodiscard]] Hit intersect(Ray ray) const {
        double lambda = ((a-ray.getLocation())*normal)/(ray.getDirection()*normal);
        if (lambda > 0) {
            auto hit = Hit(lambda, ray.getLocation() + ray.getDirection() * lambda, normal, material.getAlbedo());
            auto pbc = Triangle(hit.getPosition(), b, c);
            auto pca = Triangle(hit.getPosition(), c, a);
            auto pab = Triangle(hit.getPosition(), a, b);
            double lambdaA = pbc.getArea()/this->getArea();
            double lambdaB = pca.getArea()/this->getArea();
            double lambdaC = pab.getArea()/this->getArea();
            if (lambdaA >= 0 && lambdaB >= 0 && lambdaC >= 0 && std::abs(lambdaA + lambdaB + lambdaC - 1.0) < 1e-6) {
                return {lambda, ray.getLocation() + ray.getDirection() * lambda, normal, material.getAlbedo()};
            }
            return {-1, ray.getLocation() + ray.getDirection() * lambda, normal, material.getAlbedo()};
        }
        return {-1, ray.getLocation() + ray.getDirection() * lambda, normal, material.getAlbedo()};
    }
};

#endif //RAYTRACER_QT_OBJECT_H