#include <iostream>
#include <QApplication>
#include <QLabel>
#include <QImage>
#include <queue>

#include "Sphere.h"
#include "2d/circleVect.h"
#include "Color.h"
#include "Ray.h"
#include "2d/Vector2d.h"
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

    std::pmr::vector<Sphere> spheres;

    Ray ray(width, height);
    Sphere sphere1(Vector3d(500, 500, 200), 150, Color(Qt::red));
    Sphere sphere2(Vector3d(600, 600, 180), 150, Color(Qt::blue));
    Sphere sphere3(Vector3d(400, 600, 220), 150, Color(Qt::green));

    spheres.emplace_back(sphere1);
    spheres.emplace_back(sphere2);
    spheres.emplace_back(sphere3);

    for (int x = 0; x < width; x++) {
        ray.resetY();
        for (int y = 0; y < height; y++) {
            double lambda = std::numeric_limits<double>::infinity();
            Color color(BACKGROUND_COLOR);
            for (auto& sphere : spheres) {
                double lambda1 = sphere.intersect(ray);
                if (lambda1 >= 0 && lambda1 < lambda) {
                    lambda = lambda1;
                    color = sphere.get_color();
                }
            }
            if (lambda < std::numeric_limits<double>::infinity()) {
                image.setPixelColor(x, y, color.getColor());
            } else {
                image.setPixelColor(x, y, BACKGROUND_COLOR);
            }
            ray.incrementY();
        }
        ray.incrementX();
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