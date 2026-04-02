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

Color::Color() {
    this->r = 0;
    this->g = 0;
    this->b = 0;
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

QColor Color::getQColor() const {
    return QColor().fromRgbF(r, g, b);
}

Color Color::operator*(Color color) const {
    return Color(r*color.r, g*color.g, b*color.b);
}

Color Color::operator*(double value) const {
    return Color(r*value, g*value, b*value);
}

Color Color::operator+(Color color) const {
    return Color(
          std::min(1.0, r + color.r),
          std::min(1.0, g + color.g),
          std::min(1.0, b + color.b)
      );
}

bool Color::operator==(Color color) const {
    return r == color.r && g == color.g && b == color.b;
}
