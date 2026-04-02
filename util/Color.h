//
// Created by Nicolas Perrez on 26/02/2026.
//

#ifndef RAYTRACER_QT_COLOR_H
#define RAYTRACER_QT_COLOR_H
#include <qcolor.h>


class Color {
private:
    double r;
    double g;
    double b;

public:
    Color(double r, double g, double b);
    Color(QColor color);
    Color();

    double getR() const;
    double getG() const;
    double getB() const;
    QColor getQColor() const;

    Color operator*(Color color) const;
    Color operator*(double value) const;
    Color operator+(Color color) const;

};


#endif //RAYTRACER_QT_COLOR_H