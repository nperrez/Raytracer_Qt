#include <QApplication>
#include <QLabel>
#include <QImage>
#include <queue>
#include <execution>

#include "3dObjects/Sphere.h"
#include "util/Color.h"
#include "3dObjects/LightSource.h"
#include "util/Ray.h"
#include "util/Vector3d.h"

//Image properties
constexpr int width = 1000;
constexpr int height = 1000;
QColor BACKGROUND_COLOR = Qt::black;

Color computeColor(std::pmr::vector<LightSource> lightsources, Hit hit) {
    Color addColor = Color(0, 0, 0);
    for (auto &lightsource: lightsources) {
        Vector3d s = (lightsource.getPosition()-hit.getPosition())/(lightsource.getPosition()-hit.getPosition()).getLength();
        if (s*hit.getNormal() >= 0) {
            addColor = addColor + lightsource.getColor() * (s*hit.getNormal());
        }
    }
    return addColor;
}

void drawSpheres(QImage &image, const std::pmr::vector<Sphere> &spheres,
                 const std::pmr::vector<LightSource> &lightsources, Ray ray, int x, int y) {
    double lambda = std::numeric_limits<double>::infinity();
    Color color(BACKGROUND_COLOR);
    for (auto &sphere: spheres) {
        Hit hit = sphere.intersect(ray);
        if (hit.getLambda() >= 0 && hit.getLambda() < lambda) {
            lambda = hit.getLambda();
            Color addColor = computeColor(lightsources, hit);
            color = hit.getColor() * addColor + hit.getColor() * 0.1;
        }
    }
    if (lambda < std::numeric_limits<double>::infinity()) {
        image.setPixelColor(x, y, color.getColor());
    } else {
        image.setPixelColor(x, y, BACKGROUND_COLOR);
    }
}

std::pmr::vector<Sphere> createSpheres() {
    std::pmr::vector<Sphere> spheres;

    Sphere sphere1(Vector3d(500, 510, 230), 150, Color(1, 0, 0));
    Sphere sphere2(Vector3d(590, 500, 200), 150, Color(0, 1, 0));
    Sphere sphere3(Vector3d(550, 550, 150), 150, Color(0, 0, 1));
    Sphere sphere4(Vector3d(520, 500, 100), 80, Color(1, 1, 0));

    spheres.emplace_back(sphere1);
    spheres.emplace_back(sphere2);
    spheres.emplace_back(sphere3);
    spheres.emplace_back(sphere4);
    return spheres;
}

std::pmr::vector<LightSource> createLightsources() {
    std::pmr::vector<LightSource> lightsources;

    LightSource ls1(Vector3d(800, 500, 100), Color(1, 1, 1));
    LightSource ls2(Vector3d(100, 400, 50), Color(1, 1, 1));

    lightsources.emplace_back(ls1);
    lightsources.emplace_back(ls2);
    return lightsources;
}

int main(int argc, char *argv[]) {

    //Window Management
    QApplication a(argc, argv);
    QImage image(width, height, QImage::Format_ARGB32);

    //Scene setup
    auto spheres = createSpheres();
    auto lightsources = createLightsources();

    //Actual Raytracing stuff (+Multithreading)
    std::vector<int> xs(width);
    std::iota(xs.begin(), xs.end(), 0);

    std::for_each(std::execution::par, xs.begin(), xs.end(), [&](int x) {
        Ray ray(x, 0);

        for (int i = 0; i < x; i++) {
            ray.incrementX();
        }

        for (int y = 0; y < height; y++) {
            drawSpheres(image, spheres, lightsources, ray, x, y);
            ray.incrementY();
        }
    });

    //Window Management
    QLabel label;
    label.setPixmap(QPixmap::fromImage(image));
    label.show();
    return QApplication::exec();
}