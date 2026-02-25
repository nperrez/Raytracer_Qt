#include <QApplication>
#include <QLabel>
#include <QImage>
#include "Vector2d.h"

//Image properties
constexpr int width = 600;
constexpr int height = 600;
constexpr auto BACKGROUND_COLOR = QColor(255, 255, 255);

//Class for Circles using Vectors
class CircleVect {
public:
    Vector2d center;
    int radius;

    CircleVect(const Vector2d& center, const int radius) {
        this->center = center;
        this->radius = radius;
    }
};

//Class for Circles without Vectors
class Circle {
public:
    int radius = 50;
    int centerX = 80;
    int centerY = 180;

    Circle(int radius, int centerX, int centerY) {
        this->radius = radius;
        this->centerX = centerX;
        this->centerY = centerY;
    }
};

//Create vectors and Iterators for use with multiple Circles in one image
std::pmr::vector<Circle> circles;
std::pmr::vector<CircleVect> circleVects;
std::pmr::vector<Circle>::iterator itc;
std::pmr::vector<CircleVect>::iterator itcv;

//Draw Circles without using Vectors
void drawCircles(QImage& image) {

    circles.push_back(Circle(50, 300, 250));
    circles.push_back(Circle(100, 100, 450));

    for (int x = 0; x < width; x++) {
        for (int y = 0; y < height; y++) {

            bool inside = false;

            for (itc = circles.begin(); itc != circles.end(); ++itc) {
                int xc = x - itc->centerX;
                int yc = y - itc->centerY;
                if (xc*xc + yc*yc <= itc->radius*itc->radius) {
                    inside = true;
                    break;
                }
            }
            if (inside) {
                image.setPixelColor(x, y, QColor(255, 0, 0));
            } else {
                image.setPixelColor(x, y, BACKGROUND_COLOR);
            }
        }
    }
}

//Draw Circles using Vectors
void drawCirclesVect(QImage& image) {

    circleVects.push_back(CircleVect(Vector2d(500, 120), 140));
    circleVects.push_back(CircleVect(Vector2d(250, 360), 200));

    for (int x = 0; x < width; x++) {
        for (int y = 0; y < height; y++) {

            bool inside = false;

            for (itcv = circleVects.begin(); itcv != circleVects.end(); ++itcv) {
                Vector2d p = Vector2d(x, y);
                Vector2d c = itcv->center;
                Vector2d pc = p-c;
                if (pc.length() <= itcv->radius) {
                    inside = true;
                    break;
                }
            }
            if (inside) {
                image.setPixelColor(x, y, QColor(255, 0, 0));
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