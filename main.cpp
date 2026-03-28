#define QT_NO_KEYWORDS
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
constexpr int width = 1920;
constexpr int height = 1200;
QColor BACKGROUND_COLOR = Qt::black;
Color AMBIENT_COLOR = Color(0.1, 0.1, 0.1);

Color shadeBlinnPhong(Scene scene, Hit hit, LightSource lightSource) {
    Vector3d normal = hit.getNormal();
    Vector3d lightDirection = lightSource.getPosition() - hit.getPosition();
    Vector3d viewDirection = scene.getCamera().getForward();
    Vector3d h = (lightDirection + viewDirection).normalize() / (lightDirection + viewDirection).getLength();
    //auto material = std::dynamic_pointer_cast<BlinnPhong>(hit.getMaterial());

}

Color computeColor(Hit hit, const Scene& scene) {
    auto addColor = Color(0, 0, 0);
    for (auto &lightsource: scene.getLightSources()) {
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
            Color addColor = computeColor(hit, scene);
            color = hit.getColor() * addColor + hit.getColor() * AMBIENT_COLOR;
        }
    }
}

void calculateSpheres(const Scene &scene, const Ray &ray, double &lambda, Color &color) {
    for (auto &sphere: scene.getSpheres()) {
        Hit hit = sphere.intersect(ray);
        if (hit.getLambda() >= 0 && hit.getLambda() < lambda) {
            lambda = hit.getLambda();
            Color addColor = computeColor(hit, scene);
            color = hit.getColor() * addColor + hit.getColor() * AMBIENT_COLOR;
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
    const auto camera = Camera(Vector3d(500, 500, -300), Vector3d(300, 300, 200), 2, width, height);
    auto scene =  Scene(width, height, camera, BACKGROUND_COLOR);

    // Add Spheres
    scene.addSphere(Sphere(Vector3d(300, 300, 300), 80, Material(Lambert, Color(1.0, 0.2, 0.2))));     // Red sphere (center)
    scene.addSphere(Sphere(Vector3d(150, 200, 250), 60, Material(Lambert, Color(0.2, 1.0, 0.2))));     // Green sphere (left)
    scene.addSphere(Sphere(Vector3d(450, 250, 280), 50, Material(Lambert, Color(0.2, 0.2, 1.0))));     // Blue sphere (right)
    scene.addSphere(Sphere(Vector3d(300, 450, 320), 45, Material(Lambert, Color(1.0, 1.0, 0.2))));     // Yellow sphere (top)

    // Add Triangles (floor and walls)
    // Floor triangles (gray)
    scene.addTriangle(Triangle(Vector3d(600, 0, 400), Vector3d(0, 0, 400),  Vector3d(600, 600, 400), Material(Lambert, Color(0.6, 0.6, 0.6))));
    scene.addTriangle(Triangle(Vector3d(600, 600, 400), Vector3d(0, 0, 400),  Vector3d(0, 600, 400), Material(Lambert, Color(0.6, 0.6, 0.6))));

    // Back wall triangles (light gray)
    scene.addTriangle(Triangle(Vector3d(0, 0, 400), Vector3d(600, 0, 400), Vector3d(600, 0, 0), Material(Lambert, Color(0.7, 0.7, 0.7))));
    scene.addTriangle(Triangle(Vector3d(0, 0, 400), Vector3d(600, 0, 0), Vector3d(0, 0, 0), Material(Lambert, Color(0.7, 0.7, 0.7))));

    // Colored triangle for visual interest (magenta)
    scene.addTriangle(Triangle(Vector3d(200, 150, 350), Vector3d(350, 100, 380), Vector3d(280, 250, 360), Material(Lambert, Color(1.0, 0.4, 0.8))));

    // Add Light Sources
    scene.addLightSource(LightSource(Vector3d(200, 400, 100), Color(1.0, 1.0, 1.0)));   // Main white light (top left)
    scene.addLightSource(LightSource(Vector3d(500, 300, 50), Color(1, 1, 1.0)));    // Blue-tinted light (right)




    //Window Management
    QApplication a(argc, argv);
    QImage image(scene.getWidth(), scene.getHeight(), QImage::Format_ARGB32);

    //Actual Raytracing stuff (+Multithreading)
    std::vector<int> xs(width);
    std::iota(xs.begin(), xs.end(), 0);

    std::for_each(std::execution::par, xs.begin(), xs.end(), [&](const int x) {

        for (int y = 0; y < height; y++) {
            Ray ray = scene.getCamera().getRay(x, y);
            castRay(image, scene, ray, x, y);
        }
    });

    //Window Management
    QLabel label;
    label.setPixmap(QPixmap::fromImage(image));
    label.show();
    return QApplication::exec();
}