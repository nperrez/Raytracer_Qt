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
QColor BACKGROUND_COLOR = Qt::lightGray;

Color computeColor(std::pmr::vector<LightSource> lightsources, Hit hit, const Scene& scene) {
    auto addColor = Color(0, 0, 0);
    for (auto &lightsource: lightsources) {
        Vector3d s = (lightsource.getPosition()-hit.getPosition())/(lightsource.getPosition()-hit.getPosition()).getLength();
        if (s*hit.getNormal() >= 0) {
            bool blocked = false;
            const auto ray = Ray(hit.getPosition() + s * 10e-6, s);
            for (auto &sphere: scene.getSpheres()) {
                if (Hit hit2 = sphere.intersect(ray); hit2.getLambda() >= 0 && (lightsource.getPosition() - hit.getPosition()).getLength() > (hit2.getPosition() - hit.getPosition()).getLength()) {
                    blocked = true;
                    break;
                }
            }
            if (!blocked) {
                for (auto &triangle: scene.getTriangles()) {
                    if (Hit hit2 = triangle.intersect(ray); hit2.getLambda() >= 0 && (lightsource.getPosition() - hit.getPosition()).getLength() > (hit2.getPosition() - hit.getPosition()).getLength()) {
                        blocked = true;
                        break;
                    }
                }
            }
            if (!blocked) {
                addColor = addColor + lightsource.getColor() * (s*hit.getNormal());
            }
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
    auto scene =  Scene(width, height, BACKGROUND_COLOR);
    //scene.addSphere(Sphere(Vector3d(500, 500, 300), 200, Color(0, 1, 0)));
    //scene.addTriangle(Triangle(Vector3d(330, 350, 200), Vector3d(300, 150, 250), Vector3d(220, 350, 200), Color(0, 1, 1)));
    //scene.addTriangle(Triangle(Vector3d(300, 150, 250), Vector3d(330, 350, 200), Vector3d(900, 260, 70), Color(1, 0, 1)));
    //scene.addLightSource(LightSource(Vector3d(150, 150, 230), Color(1, 1, 1)));
    //scene.addLightSource(LightSource(Vector3d(170, 170, 180), Color(1, 1, 1)));

    scene.addTriangle(Triangle(Vector3d(800,300,0),Vector3d(-200,300,0),Vector3d(800,300,1000),Color(0.42,0.71,0.53)));
    scene.addTriangle(Triangle(Vector3d(800,300,1000),Vector3d(-200,300,0),Vector3d(-200,300,1000),Color(0.61,0.46,0.77)));

    scene.addSphere(Sphere(Vector3d(550,450,800),120,Color(0.2,0.7,0.9)));

    scene.addLightSource(LightSource(Vector3d(100,700,100),Color(1,1,1)));
    scene.addLightSource(LightSource(Vector3d(300,800,150),Color(1,1,1)));

    // cube

    scene.addTriangle(Triangle(Vector3d(320,400,360),Vector3d(230,400,420),Vector3d(400,520,420),Color(0.92,0.31,0.28)));
    scene.addTriangle(Triangle(Vector3d(400,520,420),Vector3d(230,400,420),Vector3d(310,520,480),Color(0.34,0.13,0.47)));

    scene.addTriangle(Triangle(Vector3d(350,430,500),Vector3d(260,430,560),Vector3d(430,550,560),Color(0.75,0.22,0.63)));
    scene.addTriangle(Triangle(Vector3d(430,550,560),Vector3d(260,430,560),Vector3d(340,550,620),Color(0.26,0.66,0.91)));

    scene.addTriangle(Triangle(Vector3d(230,400,420),Vector3d(260,430,560),Vector3d(340,550,620),Color(0.87,0.54,0.19)));
    scene.addTriangle(Triangle(Vector3d(340,550,620),Vector3d(230,400,420),Vector3d(310,520,480),Color(0.57,0.34,0.78)));

    scene.addTriangle(Triangle(Vector3d(350,430,500),Vector3d(320,400,360),Vector3d(430,550,560),Color(0.21,0.92,0.71)));
    scene.addTriangle(Triangle(Vector3d(430,550,560),Vector3d(320,400,360),Vector3d(400,520,420),Color(0.93,0.49,0.37)));

    scene.addTriangle(Triangle(Vector3d(260,430,560),Vector3d(230,400,420),Vector3d(350,430,500),Color(0.46,0.82,0.28)));
    scene.addTriangle(Triangle(Vector3d(350,430,500),Vector3d(230,400,420),Vector3d(320,400,360),Color(0.69,0.26,0.57)));

    scene.addTriangle(Triangle(Vector3d(400,520,420),Vector3d(310,520,480),Vector3d(430,550,560),Color(0.32,0.74,0.86)));
    scene.addTriangle(Triangle(Vector3d(430,550,560),Vector3d(310,520,480),Vector3d(340,550,620),Color(0.78,0.53,0.15)));

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
    });

    //Window Management
    QLabel label;
    label.setPixmap(QPixmap::fromImage(image));
    label.show();
    return QApplication::exec();
}