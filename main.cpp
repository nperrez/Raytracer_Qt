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

Vector3d reflect(const Vector3d &rayDirection, const Vector3d &normal) {
    return rayDirection - normal * 2 * (rayDirection * normal);
}

Hit castRay(const Ray &ray, const Scene &scene) {
    double lambda = std::numeric_limits<double>::infinity();
    Hit closestHit;

    for (auto &objects: scene.getObjects()) {
        Hit hit = objects->intersect(ray);
        if (hit.getLambda() >= 0 && hit.getLambda() < lambda) {
            lambda = hit.getLambda();
            closestHit = hit;
        }
    }

    return closestHit;
}

Color traceRay(Ray ray, const Scene &scene, int depth) {
    Color color(BACKGROUND_COLOR);
    if (depth <= 0) return color;

    Hit hit = castRay(ray, scene);

    Color diffuse = Color(0, 0, 0);
    Color specular = Color(0, 0, 0);
    Color reflective = Color(0, 0, 0);

    for (auto &lightSource: scene.getLightSources()) {
        const Vector3d l = (lightSource.getPosition() - hit.getPosition()).normalize();
        const Vector3d v = (scene.getCamera().getPosition() - hit.getPosition()).normalize();
        const Vector3d n = hit.getNormal().normalize();
        const Vector3d s = (lightSource.getPosition()-hit.getPosition())/(lightSource.getPosition()-hit.getPosition()).getLength();

        if (s*hit.getNormal() >= 0) {
            bool blocked = false;
            const auto ray = Ray(hit.getPosition() + s * 10e-6, s);
            for (auto &object: scene.getObjects()) {
                if (Hit hit2 = object->intersect(ray); hit2.getLambda() >= 0 && (lightSource.getPosition() - hit.getPosition()).getLength() > (hit2.getPosition() - hit.getPosition()).getLength()) {
                    blocked = true;
                    break;
                }
            }
            if (!blocked) {
                diffuse = diffuse + lightSource.getColor() * hit.getColor() * hit.getMaterial().getDiffuseFact() * std::max(0.0, n * l);

                if (hit.getMaterial().getSpecularFact() > 0) {
                    const Vector3d h = (l + v).normalize();
                    specular = specular + lightSource.getColor() * hit.getMaterial().getSpecularFact() * pow(
                                 std::max(0.0, n * h), hit.getMaterial().getShininess());
                }
            }
        }
    }
    if (hit.getMaterial().getReflectionFact() > 0) {
        Ray nextRay = Ray(hit.getPosition() + hit.getNormal().normalize() * 1e-6, reflect(ray.getDirection(), hit.getNormal().normalize()));
        Hit reflectedHit = castRay(nextRay, scene);
        if (reflectedHit.getLambda() >= 0) {
            reflective = traceRay(nextRay, scene, depth -1) * hit.getMaterial().getReflectionFact();
        }
    }
    return AMBIENT_COLOR * hit.getMaterial().getAmbientFact() + diffuse + specular + reflective;
}

Color shade(Hit hit, const Scene& scene) {
    auto color = Color(0, 0, 0);
    for (auto &lightsource: scene.getLightSources()) {
        Vector3d s = (lightsource.getPosition()-hit.getPosition())/(lightsource.getPosition()-hit.getPosition()).getLength();
        if (s*hit.getNormal() >= 0) {
            bool blocked = false;
            const auto ray = Ray(hit.getPosition() + s * 10e-6, s);
            for (auto &object: scene.getObjects()) {
                if (Hit hit2 = object->intersect(ray); hit2.getLambda() >= 0 && (lightsource.getPosition() - hit.getPosition()).getLength() > (hit2.getPosition() - hit.getPosition()).getLength()) {
                    blocked = true;
                    break;
                }
            }
            if (!blocked) {
                color = color + traceRay(ray, scene, 1);
            }
        }
    }
    return color;
}

int main(int argc, char *argv[]) {

    //Scene setup
    const auto camera = Camera(Vector3d(500, 500, 0), Vector3d(500, 500, 500), 1.6, width, height);
    auto scene =  Scene(width, height, camera, BACKGROUND_COLOR);

    /*// Add Spheres
    scene.addSphere(Sphere(Vector3d(300, 300, 300), 80, Material(BlinnPhong, Color(1.0, 0, 0), 25, 1, 0.1, 0.5)));     // Red sphere (center)
    scene.addSphere(Sphere(Vector3d(150, 200, 250), 60, Material(BlinnPhong, Color(0, 1, 0), 25, 1, 0.1, 1)));
    //scene.addSphere(Sphere(Vector3d(150, 200, 250), 60, Material(Lambert, Color(0, 1, 0)))); // Green sphere (left)
    scene.addSphere(Sphere(Vector3d(450, 250, 280), 50, Material(BlinnPhong, Color(0.2, 0.2, 1.0), 25, 1, 0.1, 1)));     // Blue sphere (right)
    scene.addSphere(Sphere(Vector3d(300, 450, 320), 45, Material(BlinnPhong, Color(1.0, 1.0, 0.2), 25, 1, 0.1, 1)));     // Yellow sphere (top)

    // Add Triangles (floor and walls)
    // Floor triangles (gray)
    scene.addTriangle(Triangle(Vector3d(600, 0, 400), Vector3d(0, 0, 400),  Vector3d(600, 600, 400), Material(Lambert, Color(0.6, 0.6, 0.6))));
    scene.addTriangle(Triangle(Vector3d(600, 600, 400), Vector3d(0, 0, 400),  Vector3d(0, 600, 400), Material(Lambert, Color(0.6, 0.6, 0.6))));

    // Back wall triangles (light gray)
    scene.addTriangle(Triangle(Vector3d(0, 0, 400), Vector3d(600, 0, 400), Vector3d(600, 0, 0), Material(Lambert, Color(0.7, 0.7, 0.7))));
    scene.addTriangle(Triangle(Vector3d(0, 0, 400), Vector3d(600, 0, 0), Vector3d(0, 0, 0), Material(Lambert, Color(0.7, 0.7, 0.7))));

    // Add Light Sources
    scene.addLightSource(LightSource(Vector3d(200, 400, 100), Color(1.0, 1.0, 1.0)));   // Main white light (top left)
    scene.addLightSource(LightSource(Vector3d(500, 300, 50), Color(1, 1, 1.0)));    // Blue-tinted light (right)
*/

    scene.addSphere(Vector3d(500, 500, 500), 200, Material(Color(1, 0, 0), 100, 1, 0.1, 0.8));
    scene.addSphere(Vector3d(480, 480, 350), 100, Material(Color(0, 0, 1), 500, 1, 0.1, 1));

    scene.addSphere(Vector3d(590, 490, 330), 50, Material(Color(0, 0, 0), 1));
    scene.addSphere(Vector3d(560, 490, 270), 10, Material(Color(0, 1, 0), 100, 1, 0.1, 1));

    scene.addLightSource(LightSource(Vector3d(700, 600, 0), Color(0.8, 0.8, 0.8)));
    scene.addLightSource(LightSource(Vector3d(300, 600, 0), Color(1, 1, 1)));




    //Window Management
    QApplication a(argc, argv);
    QImage image(scene.getWidth(), scene.getHeight(), QImage::Format_ARGB32);

    //Actual Raytracing stuff (+Multithreading)
    std::vector<int> xs(width);
    std::iota(xs.begin(), xs.end(), 0);

    std::for_each(std::execution::par, xs.begin(), xs.end(), [&](const int x) {

        for (int y = 0; y < height; y++) {
            Ray ray = scene.getCamera().getRay(x, y);
            Color color = traceRay(ray, scene, 10);
            image.setPixelColor(x, y, color.getQColor());
        }
    });

    //Window Management
    QLabel label;
    label.setPixmap(QPixmap::fromImage(image));
    label.show();
    return QApplication::exec();
}