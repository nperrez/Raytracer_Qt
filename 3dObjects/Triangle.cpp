//
// Created by nicolas on 12.03.26.
//

#include "Triangle.h"

Triangle::Triangle(Vector3d a, Vector3d b, Vector3d c, Color color) : a(a), b(b), c(c), color(color) {
    v = b-a;
    w = c-a;
}

Vector3d Triangle::getA() {
    return a;
}

Vector3d Triangle::getB() {
    return b;
}

Vector3d Triangle::getC() {
    return c;
}

Color Triangle::getColor() {
    return color;
}