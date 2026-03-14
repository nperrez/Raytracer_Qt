#include <QApplication>
#include <QLabel>
#include <QImage>
#include <queue>
#include <execution>

#include "3dObjects/Sphere.h"
#include "util/Color.h"
#include "3dObjects/LightSource.h"
#include "3dObjects/Triangle.h"
#include "util/Ray.h"
#include "util/Scene.h"
#include "util/Vector3d.h"

//Image properties
constexpr int width = 1000;
constexpr int height = 1000;
QColor BACKGROUND_COLOR = Qt::black;

Color computeColor(std::pmr::vector<LightSource> lightsources, Hit hit, const Scene& scene) {
    Color addColor = Color(0, 0, 0);
    for (auto &lightsource: lightsources) {
        Vector3d s = (lightsource.getPosition()-hit.getPosition())/(lightsource.getPosition()-hit.getPosition()).getLength();
        if (s*hit.getNormal() >= 0) {
            Ray ray = Ray(hit.getPosition() + s * 10e-6, s);
            for (auto &sphere: scene.getSpheres()) {
                Hit hit2 = sphere.intersect(ray);
                if (hit2.getLambda() >= 0) {
                    return {0, 0, 0};
                }
            }
            for (auto &triangle: scene.getTriangles()) {
                Hit hit2 = triangle.intersect(ray);
                if (hit2.getLambda() >= 0) {
                    return {0, 0, 0};
                }
            }
            addColor = addColor + lightsource.getColor() * (s*hit.getNormal());
        }
    }
    return addColor;
}

void calculateTriangles(const Scene &scene, const Ray &ray, double &lambda, Color &color) {
    for (auto &triangle: scene.getTriangles()) {
        Hit hit = triangle.intersect(ray);
        if (hit.getLambda() >= 0 && hit.getLambda() < lambda) {
            lambda = hit.getLambda();
            Color addColor = computeColor(scene.getLightSources(), hit, scene);
            color = hit.getColor() * addColor + hit.getColor() * 0.1;
        }
    }
}

void calculateSpheres(const Scene &scene, const Ray &ray, double &lambda, Color &color) {
    for (auto &sphere: scene.getSpheres()) {
        Hit hit = sphere.intersect(ray);
        if (hit.getLambda() >= 0 && hit.getLambda() < lambda) {
            lambda = hit.getLambda();
            Color addColor = computeColor(scene.getLightSources(), hit, scene);
            color = hit.getColor() * addColor + hit.getColor() * 0.1;
        }
    }
}

void castRay(QImage &image, const Scene &scene, const Ray &ray, int x, int y) {
    double lambda = std::numeric_limits<double>::infinity();
    Color color(BACKGROUND_COLOR);
    calculateSpheres(scene, ray, lambda, color);
    calculateTriangles(scene, ray, lambda, color);
    if (lambda < std::numeric_limits<double>::infinity()) {
        image.setPixelColor(x, y, color.getColor());
    } else {
        image.setPixelColor(x, y, BACKGROUND_COLOR);
    }
}

int main(int argc, char *argv[]) {

    //Scene setup
    Scene scene =  Scene(width, height, BACKGROUND_COLOR);
    scene.addSphere(Sphere(Vector3d(500, 500, 400), 180, Color(1, 0, 0)));
    scene.addTriangle(Triangle(Vector3d(300, 400, 200), Vector3d(200, 100, 100), Vector3d(350, 300, 250), Color(0, 1, 1)));
    scene.addTriangle(Triangle(Vector3d(500, 400, 400), Vector3d(350, 300, 250), Vector3d(200, 100, 100), Color(1, 0, 1)));
    scene.addTriangle(Triangle(Vector3d(300, 400, 200), Vector3d(350, 300, 250), Vector3d(500, 400, 400), Color(0, 1, 0)));
    scene.addLightSource(LightSource(Vector3d(100, 100, 100), Color(1, 1, 1)));
    scene.addLightSource(LightSource(Vector3d(60, 120, -100), Color(1, 1, 1)));

    //Window Management
    QApplication a(argc, argv);
    QImage image(scene.getWidth(), scene.getHeight(), QImage::Format_ARGB32);

    //Actual Raytracing stuff (+Multithreading)
    std::vector<int> xs(width);
    std::iota(xs.begin(), xs.end(), 0);

    std::for_each(std::execution::par, xs.begin(), xs.end(), [&](const int x) {
        Ray ray(x, 0);

        for (int y = 0; y < height; y++) {
            castRay(image, scene, ray, x, y);
            ray.incrementY();
        }

        for (int i = 0; i < x; i++) {
            ray.incrementX();
        }
    });

    //Window Management
    QLabel label;
    label.setPixmap(QPixmap::fromImage(image));
    label.show();
    return QApplication::exec();
}