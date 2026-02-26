#include <iostream>
#include <QApplication>
#include <QLabel>
#include <QImage>

#include "Sphere.h"
#include "circleVect.h"
#include "Color.h"
#include "Ray.h"
#include "Vector2d.h"
#include "Vector3d.h"

//Image properties
constexpr int width = 1000;
constexpr int height = 1000;
QColor BACKGROUND_COLOR = Qt::black;

//Draw Circles using Vectors
void drawCirclesVect(QImage& image) {

    std::pmr::vector<CircleVect> circleVects;
    std::pmr::vector<CircleVect>::iterator itcv;

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

void drawSpheres(QImage& image) {

    for (int x = 0; x < width; x++) {
        for (int y = 0; y < height; y++) {
            Sphere sphere(Vector3d(500, 500, 200), 150, Ray(x, y));
            double t = sphere.getLambda();
            if (t >= 0) {
                image.setPixelColor(x, y, sphere.getLambda());
            } else {
                image.setPixelColor(x, y, BACKGROUND_COLOR);
            }
        }
    }
}

int main(int argc, char *argv[]) {

    //Window/App Management
    QApplication a(argc, argv);
    QImage image(width, height, QImage::Format_ARGB32);

    //Insert draw function here
    drawSpheres(image);

    //Window Management
    QLabel label;
    label.setPixmap(QPixmap::fromImage(image));
    label.show();
    return QApplication::exec();
}