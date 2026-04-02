#define QT_NO_KEYWORDS
#include <QApplication>
#include <QLabel>
#include <QImage>
#include <queue>
#include <execution>
#include <iostream>
#include <random>

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

Vector3d randomVect() {
    static std::mt19937 rng(std::random_device{}()); // seed once
    static std::uniform_real_distribution dist(-1.0, 1.0);
    double x = dist(rng);
    double y = dist(rng);
    double z = dist(rng);
    double len = sqrt(x*x + y*y + z*z);
    return {x/len, y/len, z/len};
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

Color traceRay(Ray initialRay, const Scene &scene, int depth) {
    Color color(BACKGROUND_COLOR);
    if (depth <= 0) return color;

    Hit hit = castRay(initialRay, scene);
    if (hit.getLambda() <= 0) return color;

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
                    specular = specular + lightSource.getColor() * hit.getMaterial().getSpecularFact() * pow(std::max(0.0, n * h), hit.getMaterial().getShininess());
                }
            }
        }
    }
    if (hit.getMaterial().getReflectionFact() > 0) {
        if (hit.getMaterial().getGlossiness() == 0) {
            Ray nextRay = Ray(hit.getPosition() + hit.getNormal().normalize() * 1e-6, reflect(initialRay.getDirection(), hit.getNormal().normalize()));
            Hit reflectedHit = castRay(nextRay, scene);
            if (reflectedHit.getLambda() >= 0) {
                reflective = traceRay(nextRay, scene, depth -1) * hit.getMaterial().getReflectionFact() * hit.getMaterial().getSpecular();
            }
        } else {
            const int iterations = 64;
            double glossyR = 0;
            double glossyG = 0;
            double glossyB = 0;
            Vector3d reflectiveDir = reflect(initialRay.getDirection(), hit.getNormal().normalize());
            Vector3d n = hit.getNormal().normalize();
            for (int i = 0; i < iterations; i++) {
                Vector3d spreadDir = (reflectiveDir + randomVect() * hit.getMaterial().getGlossiness()).normalize();
                if (spreadDir * n < 0) spreadDir =  n * -(spreadDir * n);
                Ray nextRay = Ray(hit.getPosition() + n * 1e-6, spreadDir);
                Hit reflectedHit = castRay(nextRay, scene);
                if (reflectedHit.getLambda() >= 0) {
                    Color reflectiveGlossy = traceRay(nextRay, scene, depth -1) * hit.getMaterial().getReflectionFact() * hit.getMaterial().getSpecular();
                    glossyR = glossyR + reflectiveGlossy.getR();
                    glossyG = glossyG + reflectiveGlossy.getG();
                    glossyB = glossyB + reflectiveGlossy.getB();
                }
            }
            reflective = Color(glossyR/iterations, glossyG/iterations, glossyB/iterations) * hit.getMaterial().getReflectionFact() * hit.getMaterial().getSpecular();
        }
    }
    return AMBIENT_COLOR * hit.getMaterial().getAmbientFact() + diffuse + specular + reflective;
}

int main(int argc, char *argv[]) {

    //Scene setup
    // Room: 1600 x 1000 x 1000 (width x height x depth)
    // Camera 1/8 into the room from the front, centered, FOV sized so the back wall fills the frame exactly.
    // Y=0 is ceiling (top of frame), Y=1000 is floor (bottom of frame) — matches the camera's up=(0,-1,0) convention.
    // FOV = 2*atan(500/875) so that at Z=1000 the view spans exactly X=[0,1600] and Y=[0,1000].
    const auto camera = Camera(Vector3d(800, 500, 50), Vector3d(800, 500, 500), 1.8383, width, height);
    auto scene = Scene(width, height, camera, BACKGROUND_COLOR);

    // Wall materials: specularFact=0, reflectionFact=0 (default in this constructor)
    Material wallBlue  = Material(Color(0.1, 0.1, 0.8), 0, 0.0, 0.2, 1.0);
    Material wallRed   = Material(Color(0.8, 0.1, 0.1), 0, 0.0, 0.2, 1.0);
    Material wallWhite = Material(Color(1, 1, 1), 0, 0.0, 0.2, 1.0);

    // Left wall (blue, X=0, normal +X)
    scene.addTriangle(Vector3d(0,0,0),    Vector3d(0,1000,0),    Vector3d(0,0,1000),    wallBlue);
    scene.addTriangle(Vector3d(0,1000,0), Vector3d(0,1000,1000), Vector3d(0,0,1000),    wallBlue);
    // Right wall (red, X=1600, normal -X)
    scene.addTriangle(Vector3d(1600,0,0),    Vector3d(1600,0,1000),    Vector3d(1600,1000,0),    wallRed);
    scene.addTriangle(Vector3d(1600,0,1000), Vector3d(1600,1000,1000), Vector3d(1600,1000,0),    wallRed);
    // Back wall (white, Z=1000, normal -Z)
    scene.addTriangle(Vector3d(0,0,1000),    Vector3d(0,1000,1000),    Vector3d(1600,0,1000),    wallWhite);
    scene.addTriangle(Vector3d(1600,0,1000), Vector3d(0,1000,1000),    Vector3d(1600,1000,1000), wallWhite);
    // Front Wall
    scene.addTriangle( Vector3d(0,1000,0),   Vector3d(0,0,0),    Vector3d(1600,0,0),    wallWhite);
    scene.addTriangle(Vector3d(0,1000,0),  Vector3d(1600,0,0),   Vector3d(1600,1000,0), wallWhite);
    // Ceiling (white, Y=0, normal +Y)
    scene.addTriangle(Vector3d(0,0,0),    Vector3d(0,0,1000),    Vector3d(1600,0,0),    wallWhite);
    scene.addTriangle(Vector3d(1600,0,0), Vector3d(0,0,1000),    Vector3d(1600,0,1000), wallWhite);
    // Floor (white, Y=1000, normal -Y)
    scene.addTriangle(Vector3d(0,1000,0),    Vector3d(1600,1000,0),    Vector3d(0,1000,1000),    wallWhite);
    scene.addTriangle(Vector3d(1600,1000,0), Vector3d(1600,1000,1000), Vector3d(0,1000,1000),    wallWhite);

    //scene.addLightSource(LightSource(Vector3d(800, 100, 500), Color(1, 1, 1)));
    scene.addLightSource(LightSource(Vector3d(600, 100, 500), Color(0.8, 0.8, 0.8)));
    scene.addLightSource(LightSource(Vector3d(1000, 100, 500), Color(0.8, 0.8, 0.8)));

    scene.addSphere(Vector3d(800, 600, 600), 200, Material(Color(0.8, 0.7, 0.1), 0));
    //scene.addSphere(Vector3d(800, 600, 600), 200, Material(Color(0.5, 0.4, 0.1), Color(0.5, 0.4, 0.1), 50, 0.1, 0.2, 0, 0, 0.8));
    //scene.addSphere(Vector3d(800, 600, 600), 200, Material(Color(0.8, 0, 0.4), 100, 1, 0.1, 0.8));

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