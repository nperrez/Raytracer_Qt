#include <iostream>
#include <QApplication>
#include <QLabel>
#include <QImage>
#include <queue>

#include "Sphere.h"
#include "Color.h"
#include "LightSource.h"
#include "Ray.h"
#include "Vector3d.h"

//Image properties
constexpr int width = 1000;
constexpr int height = 1000;
QColor BACKGROUND_COLOR = Qt::black;


void drawSpheres(QImage &image) {
    std::pmr::vector<Sphere> spheres;
    std::pmr::vector<LightSource> lightsources;

    Ray ray(width, height);
    LightSource ls1(Vector3d(800, 500, 100), Color(1, 1, 1));
    LightSource ls2(Vector3d(100, 400, 50), Color(1, 1, 1));
    Sphere sphere1(Vector3d(500, 510, 230), 150, Color(1, 0, 0));
    Sphere sphere2(Vector3d(590, 500, 200), 150, Color(0, 1, 0));
    Sphere sphere3(Vector3d(550, 550, 150), 150, Color(0, 0, 1));
    Sphere sphere4(Vector3d(520, 500, 100), 80, Color(1, 1, 0));

    lightsources.emplace_back(ls1);
    lightsources.emplace_back(ls2);

    spheres.emplace_back(sphere1);
    spheres.emplace_back(sphere2);
    spheres.emplace_back(sphere3);
    spheres.emplace_back(sphere4);

    for (int x = 0; x < width; x++) {
        ray.resetY();
        for (int y = 0; y < height; y++) {
            double lambda = std::numeric_limits<double>::infinity();
            Color color(BACKGROUND_COLOR);
            for (auto &sphere: spheres) {
                Hit hit = sphere.intersect(ray);
                if (hit.getLambda() >= 0 && hit.getLambda() < lambda) {
                    lambda = hit.getLambda();
                    Color addColor = Color(0, 0, 0);
                    for (auto &lightsource: lightsources) {
                        Vector3d s = (lightsource.getPosition()-hit.getPosition())/(lightsource.getPosition()-hit.getPosition()).getLength();
                        if (s*hit.getNormal() >= 0) {
                            addColor = addColor + lightsource.getColor() * (s*hit.getNormal());
                        }
                        //std::cout << addColor.getR() << " " << addColor.getG() << " " << addColor.getB() << std::endl;
                    }
                    color = hit.getColor() * addColor + hit.getColor()*0.1;
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