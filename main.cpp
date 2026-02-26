#include <iostream>
#include <QApplication>
#include <QLabel>
#include <QImage>

#include "Color.h"
#include "Vector2d.h"

//Image properties
constexpr int width = 1000;
constexpr int height = 1000;
QColor BACKGROUND_COLOR = QColor(0, 0, 0);

QColor c();

//Class for Circles using Vectors
class CircleVect {
private:
    Vector2d center;
    int radius;
    Color color;

public:
    CircleVect(const Vector2d& center, const int radius, Color color) : radius(radius), color(color) {
        this->center = center;
    }

    Vector2d getCenter() const {
        return this->center;
    }
    int getRadius() const {
        return this->radius;
    }
    QColor getColor() const {
        return color.getColor();
    }
};

//Class for Circles without Vectors
class Circle {
private:
    int radius;
    int centerX;
    int centerY;
    Color color;

public:
    Circle(int radius, int centerX, int centerY, Color color) : color(color) {
        this->radius = radius;
        this->centerX = centerX;
        this->centerY = centerY;
    }

    int getRadius() const {
        return this->radius;
    }
    int getCenterX() const {
        return this->centerX;
    }
    int getCenterY() const {
        return this->centerY;
    }
    QColor getColor() const {
        return color.getColor();
    }
};

//Create vectors and Iterators for use with multiple Circles in one image
std::pmr::vector<Circle> circles;
std::pmr::vector<CircleVect> circleVects;
std::pmr::vector<Circle>::iterator itc;
std::pmr::vector<CircleVect>::iterator itcv;

//Draw Circles without using Vectors
void drawCircles(QImage& image) {

    circles.emplace_back(50, 300, 250, Color(1, 0, 0));
    circles.emplace_back(100, 100, 450, Color(0, 0, 1));

    for (int x = 0; x < width; x++) {
        for (int y = 0; y < height; y++) {

            bool inside = false;

            for (itc = circles.begin(); itc != circles.end(); ++itc) {
                int xc = x - itc->getCenterX();
                int yc = y - itc->getCenterY();
                if (xc*xc + yc*yc <= itc->getRadius()*itc->getRadius()) {
                    inside = true;
                    break;
                }
            }
            if (inside) {
                image.setPixelColor(x, y, itc->getColor());
            } else {
                image.setPixelColor(x, y, BACKGROUND_COLOR);
            }
        }
    }
}

//Draw Circles using Vectors
void drawCirclesVect(QImage& image) {

    circleVects.emplace_back(Vector2d(200, 400), 150, Color(0, 1, 0));
    circleVects.emplace_back(Vector2d(400, 400), 150, Color(0, 0, 1));
    circleVects.emplace_back(Vector2d(300, 573), 150, Color(1, 0, 0));

    for (int x = 0; x < width; x++) {
        for (int y = 0; y < height; y++) {

            bool inside = false;
            Color sumColor(0, 0, 0);

            for (auto it = circleVects.begin(); it != circleVects.end(); ++it) {

                Vector2d p(x, y);
                Vector2d pc = p - it->getCenter();

                if (pc.length() <= it->getRadius()) {
                    inside = true;
                    sumColor = sumColor + it->getColor();
                }
            }

            if (inside) {
                image.setPixelColor(x, y, sumColor.getColor());
            } else {
                image.setPixelColor(x, y, BACKGROUND_COLOR);
            }
        }
    }
}

int main(int argc, char *argv[]) {

    //WindowAp Management
    QApplication a(argc, argv);
    QImage image(width, height, QImage::Format_ARGB32);

    //Insert draw function here
    drawCirclesVect(image);

    //Window Management
    QLabel label;
    label.setPixmap(QPixmap::fromImage(image));
    label.show();
    return QApplication::exec();
}