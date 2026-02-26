//
// Created by Nicolas Perrez on 26/02/2026.
//

#include "Color.h"

Color::Color(double r, double g, double b) {
    this->r = r;
    this->g = g;
    this->b = b;
}

Color::Color(QColor color) {
    this->r = color.red();
    this->g = color.green();
    this->b = color.blue();
}

double Color::getR() const {
    return r;
}

double Color::getG() const {
    return g;
}

double Color::getB() const {
    return b;
}

QColor Color::getColor() const {
    return QColor().fromRgbF(r, g, b);
}

Color Color::operator*(Color color) const {
    return Color(r*color.r, g*color.g, b*color.b);
}

Color Color::operator+(Color color) const {
    if (r + color.r <= 1 && g + color.g <= 1 && b + color.b <= 1) {
        return Color(r + color.r, g + color.g, b + color.b);
    } else if (r + color.r > 1) {
        return Color(1, g + color.g, b + color.b);
    } else if (g + color.g > 1) {
        return Color(r + color.r, 1, b + color.b);
    } else if (b + color.b > 1) {
        return Color(r + color.r, g + color.g, 1);
    }
}