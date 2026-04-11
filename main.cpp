#define QT_NO_KEYWORDS
#include <QApplication>
#include <QLabel>
#include <QImage>
#include <queue>
#include <execution>
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
constexpr int MAX_DEPTH = 10;
constexpr int RANDOM_ITERATIONS = 64;

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

double randomFloat() {
    static std::mt19937 rng(std::random_device{}());
    static std::uniform_real_distribution<double> dist(0.0, 1.0);
    return dist(rng);
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

std::optional<Vector3d> refract(Vector3d i, Vector3d n, double eta1, double eta2) {
    double eta = eta1/eta2;
    double cosTheta1 = -(i * n);

    double k = 1 - eta * eta * (1 - cosTheta1 * cosTheta1);

    if (k < 0) {
        return {};
    }
    Vector3d r = i * eta + n * (eta * cosTheta1 - sqrt(k));
    return r;
}

double fresnel(double cosA, double eta1, double eta2) {
    double f0 = pow((eta1 - eta2) / (eta1 + eta2), 2);
    return f0 + (1 - f0) * pow(1 - cosA, 5);
}

Color traceRay(Ray initialRay, const Scene &scene, int depth, double eta) {
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
                if (object->getMaterial().getType() == DIELECTRIC) continue;
                if (Hit hit2 = object->intersect(ray); hit2.getLambda() >= 0 && (lightSource.getPosition() - hit.getPosition()).getLength() > (hit2.getPosition() - hit.getPosition()).getLength()) {
                    blocked = true;
                    break;
                }
            }
            if (!blocked) {
                diffuse = diffuse + lightSource.getColor() * hit.getColor() * hit.getMaterial().getDiffuseFact() * std::max(0.0, n * l);

                if (hit.getMaterial().getType() == SPECULAR) {
                    const Vector3d h = (l + v).normalize();
                    specular = specular + lightSource.getColor() * hit.getMaterial().getSpecularFact() * pow(std::max(0.0, n * h), hit.getMaterial().getShininess());
                }
            }
        }
    }
    if (hit.getMaterial().getType() == METAL) {
        if (hit.getMaterial().getGlossiness() == 0) {
            Ray nextRay = Ray(hit.getPosition() + hit.getNormal().normalize() * 1e-6, reflect(initialRay.getDirection(), hit.getNormal().normalize()));
            Hit reflectedHit = castRay(nextRay, scene);
            if (reflectedHit.getLambda() >= 0) {
                reflective = traceRay(nextRay, scene, depth -1, 1) * hit.getMaterial().getSpecular();
            }
        } else {
            double glossyR = 0;
            double glossyG = 0;
            double glossyB = 0;
            Vector3d reflectiveDir = reflect(initialRay.getDirection(), hit.getNormal().normalize());
            Vector3d n = hit.getNormal().normalize();
            for (int i = 0; i < RANDOM_ITERATIONS; i++) {
                Vector3d spreadDir = (reflectiveDir + randomVect() * hit.getMaterial().getGlossiness()).normalize();
                if (spreadDir * n < 0) spreadDir =  n * -(spreadDir * n);
                Ray nextRay = Ray(hit.getPosition() + n * 1e-6, spreadDir);
                Hit reflectedHit = castRay(nextRay, scene);
                if (reflectedHit.getLambda() >= 0) {
                    Color reflectiveGlossy = traceRay(nextRay, scene, depth -1, 1) * hit.getMaterial().getSpecular();
                    glossyR = glossyR + reflectiveGlossy.getR();
                    glossyG = glossyG + reflectiveGlossy.getG();
                    glossyB = glossyB + reflectiveGlossy.getB();
                }
            }
            reflective = Color(glossyR/RANDOM_ITERATIONS, glossyG/RANDOM_ITERATIONS, glossyB/RANDOM_ITERATIONS) * hit.getMaterial().getSpecular();
        }
    }
    if (hit.getMaterial().getType() == DIELECTRIC) {
        Vector3d n = hit.getNormal().normalize();
        Vector3d i = initialRay.getDirection().normalize();
        double eta1;
        double eta2;
        if (hit.isFrontFace()) {
            eta1 = eta;
            eta2 = hit.getMaterial().getRefractiveIndex();
        } else {
            eta1 = hit.getMaterial().getRefractiveIndex();
            eta2 = eta;
        }
        Vector3d reflectedDir = reflect(i, n);
        std::optional<Vector3d> refractedDir = refract(i, n, eta1, eta2);
        double cosA = std::clamp(-(i * n), 0.0, 1.0);
        double f = fresnel(cosA, eta1, eta2);

        if (hit.getMaterial().getGlossiness() == 0) {
            // Perfect glass: deterministic reflect + refract blend (no branching)
            Ray reflectedRay = Ray(hit.getPosition() + n * 1e-6, reflectedDir);
            Color reflectedColor = traceRay(reflectedRay, scene, depth - 1, hit.getMaterial().getRefractiveIndex());
            if (!refractedDir) return reflectedColor;
            Ray refractedRay(hit.getPosition() + (*refractedDir) * 1e-6, *refractedDir);
            Color refractedColor = traceRay(refractedRay, scene, depth - 1, 1);
            if (!hit.isFrontFace()) {
                double r = exp(hit.getMaterial().getAbsorption().getR() * hit.getLambda() * -1);
                double g = exp(hit.getMaterial().getAbsorption().getG() * hit.getLambda() * -1);
                double b = exp(hit.getMaterial().getAbsorption().getB() * hit.getLambda() * -1);
                refractedColor = refractedColor * Color(r, g, b);
            }
            return reflectedColor * f + refractedColor * (1 - f);
        }
        // Frosted glass: stochastic sampling — each iteration picks reflect OR refract,
        // never both. Branching factor stays at RANDOM_ITERATIONS (not 1+RANDOM_ITERATIONS).
        double glossyR = 0, glossyG = 0, glossyB = 0;
        for (int j = 0; j < RANDOM_ITERATIONS; j++) {
            Color sampleColor;
            if (!refractedDir || randomFloat() < f) {
                Vector3d spreadDir = (reflectedDir + randomVect() * hit.getMaterial().getGlossiness()).normalize();
                if (spreadDir * n < 0) spreadDir = n * -(spreadDir * n);
                Ray sampleRay = Ray(hit.getPosition() + n * 1e-6, spreadDir);
                sampleColor = traceRay(sampleRay, scene, depth - 1, hit.getMaterial().getRefractiveIndex());
            } else {
                Vector3d spreadDir = ((*refractedDir) + randomVect() * hit.getMaterial().getGlossiness()).normalize();
                Ray sampleRay = Ray(hit.getPosition() + spreadDir * 1e-6, spreadDir);
                sampleColor = traceRay(sampleRay, scene, depth - 1, 1);
                if (!hit.isFrontFace()) {
                    double r = exp(hit.getMaterial().getAbsorption().getR() * hit.getLambda() * -1);
                    double g = exp(hit.getMaterial().getAbsorption().getG() * hit.getLambda() * -1);
                    double b = exp(hit.getMaterial().getAbsorption().getB() * hit.getLambda() * -1);
                    sampleColor = sampleColor * Color(r, g, b);
                }
            }
            glossyR += sampleColor.getR();
            glossyG += sampleColor.getG();
            glossyB += sampleColor.getB();
        }
        return Color(glossyR / RANDOM_ITERATIONS, glossyG / RANDOM_ITERATIONS, glossyB / RANDOM_ITERATIONS);
    }
    return AMBIENT_COLOR * hit.getMaterial().getAmbientFact() + diffuse + specular + reflective;
}

int main(int argc, char *argv[]) {

    //Scene setup
    // Room: 1600 x 1000 x 1000 (width x height x depth), centered at (0,0,0).
    // X: -800..800, Y: -500..500 (Y=-500 ceiling, Y=500 floor), Z: -500..500
    // Camera sits at Z=-450 (near front wall), looks toward origin.
    // FOV = 2*atan(500/875) so the back wall fills the frame exactly.
    const auto camera = Camera(Vector3d(0, 0, -450), Vector3d(0, 0, 0), 1.8383, width, height);
    auto scene = Scene(width, height, camera, BACKGROUND_COLOR);

    // Wall materials
    Material wallBlue  = Material(Color(0.1, 0.1, 0.8), 0, 0.0, 0.2, 1.0);
    Material wallRed   = Material(Color(0.8, 0.1, 0.1), 0, 0.0, 0.2, 1.0);
    Material wallWhite = Material(Color(0.9, 0.9, 0.9), 0, 0.0, 0.2, 1.0);

    // Left wall (blue, X=-800, normal +X)
    scene.addTriangle(Vector3d(-800,-500,-500), Vector3d(-800, 500,-500), Vector3d(-800,-500, 500), wallBlue);
    scene.addTriangle(Vector3d(-800, 500,-500), Vector3d(-800, 500, 500), Vector3d(-800,-500, 500), wallBlue);
    // Right wall (red, X=800, normal -X)
    scene.addTriangle(Vector3d( 800,-500,-500), Vector3d( 800,-500, 500), Vector3d( 800, 500,-500), wallRed);
    scene.addTriangle(Vector3d( 800,-500, 500), Vector3d( 800, 500, 500), Vector3d( 800, 500,-500), wallRed);
    // Back wall (white, Z=500, normal -Z)
    scene.addTriangle(Vector3d(-800,-500, 500), Vector3d(-800, 500, 500), Vector3d( 800,-500, 500), wallWhite);
    scene.addTriangle(Vector3d( 800,-500, 500), Vector3d(-800, 500, 500), Vector3d( 800, 500, 500), wallWhite);
    // Front wall (white, Z=-500, normal +Z)
    scene.addTriangle(Vector3d(-800, 500,-500), Vector3d(-800,-500,-500), Vector3d( 800,-500,-500), wallWhite);
    scene.addTriangle(Vector3d(-800, 500,-500), Vector3d( 800,-500,-500), Vector3d( 800, 500,-500), wallWhite);
    // Ceiling (white, Y=-500, normal +Y)
    scene.addTriangle(Vector3d(-800,-500,-500), Vector3d(-800,-500, 500), Vector3d( 800,-500,-500), wallWhite);
    scene.addTriangle(Vector3d( 800,-500,-500), Vector3d(-800,-500, 500), Vector3d( 800,-500, 500), wallWhite);
    // Floor (white, Y=500, normal -Y)
    scene.addTriangle(Vector3d(-800, 500,-500), Vector3d( 800, 500,-500), Vector3d(-800, 500, 500), wallWhite);
    scene.addTriangle(Vector3d( 800, 500,-500), Vector3d( 800, 500, 500), Vector3d(-800, 500, 500), wallWhite);

    //scene.addLightSource(LightSource(Vector3d(0, -400, 0), Color(1, 1, 1)));
    scene.addLightSource(LightSource(Vector3d(-200, -400, 0), Color(0.8, 0.8, 0.8)));
    scene.addLightSource(LightSource(Vector3d( 200, -400, 0), Color(0.8, 0.8, 0.8)));
    scene.addLightSource(LightSource(Vector3d(   0,  100, -430), Color(0.3, 0.3, 0.3)));

    scene.addSphere(Vector3d( 500,  250, 100), 150, Material(Color(0.2, 0.7, 0.9), 0.1));
    //scene.addSphere(Vector3d(-300,  100, 100), 200, Material(1.04, 0, Color(0.0005, 0.0005, 0.005)));
    //scene.addSphere(Vector3d( 100,  -50, -50), 170, Material(Color(0.2, 0.8, 0.3), 50, 1, 0.1, 0.9));
    scene.addSphere(Vector3d(-300,  400, 350), 100, Material(Color(0.9, 0.9, 0.9), 0));
    //scene.addSphere(Vector3d(   0,    0,-400),  25, Material(1.6, 0, Color(0, 0, 0)));
    scene.addSphere(Vector3d(-20, 80, 300), 75, Material(Color(0.8, 0.1, 0.2), 100, 1, 0.1, 1));

    /*// Cube centered at (150, 200, -200), side length 150
    Material cubeMat = Material(1.05, 0, Color(0.005, 0.001, 0.001));
    // Front face (Z=-275, normal -Z)
    scene.addTriangle(Vector3d( 75, 125,-275), Vector3d(225, 275,-275), Vector3d(225, 125,-275), cubeMat);
    scene.addTriangle(Vector3d( 75, 125,-275), Vector3d( 75, 275,-275), Vector3d(225, 275,-275), cubeMat);
    // Back face (Z=-125, normal +Z)
    scene.addTriangle(Vector3d( 75, 125,-125), Vector3d(225, 125,-125), Vector3d(225, 275,-125), cubeMat);
    scene.addTriangle(Vector3d( 75, 125,-125), Vector3d(225, 275,-125), Vector3d( 75, 275,-125), cubeMat);
    // Left face (X=75, normal -X)
    scene.addTriangle(Vector3d( 75, 125,-275), Vector3d( 75, 125,-125), Vector3d( 75, 275,-275), cubeMat);
    scene.addTriangle(Vector3d( 75, 275,-275), Vector3d( 75, 125,-125), Vector3d( 75, 275,-125), cubeMat);
    // Right face (X=225, normal +X)
    scene.addTriangle(Vector3d(225, 125,-275), Vector3d(225, 275,-275), Vector3d(225, 125,-125), cubeMat);
    scene.addTriangle(Vector3d(225, 275,-275), Vector3d(225, 275,-125), Vector3d(225, 125,-125), cubeMat);
    // Top face (Y=125, normal -Y)
    scene.addTriangle(Vector3d( 75, 125,-275), Vector3d(225, 125,-275), Vector3d( 75, 125,-125), cubeMat);
    scene.addTriangle(Vector3d(225, 125,-275), Vector3d(225, 125,-125), Vector3d( 75, 125,-125), cubeMat);
    // Bottom face (Y=275, normal +Y)
    scene.addTriangle(Vector3d( 75, 275,-275), Vector3d( 75, 275,-125), Vector3d(225, 275,-275), cubeMat);
    scene.addTriangle(Vector3d(225, 275,-275), Vector3d( 75, 275,-125), Vector3d(225, 275,-125), cubeMat);*/

    /*scene.addSphere(Vector3d(0, 200, 0), 150, Material(1.1, 0.1, Color(0, 0, 0)));
    scene.addSphere(Vector3d(-20, 250, 300), 75, Material(Color(0.2, 0.4, 1), 0.1, 1));*/

    scene.addMesh(Mesh::fromObj("/home/nicolas/Desktop/Bean1.obj", 100, true, Material(1.2, 0, Color(0.01, 0.003, 0))));

    //Window Management
    QApplication a(argc, argv);
    QImage image(scene.getWidth(), scene.getHeight(), QImage::Format_ARGB32);

    //Actual Raytracing stuff (+Multithreading)
    std::vector<int> xs(width);
    std::iota(xs.begin(), xs.end(), 0);

    std::for_each(std::execution::par, xs.begin(), xs.end(), [&](const int x) {

        for (int y = 0; y < height; y++) {
            Ray ray = scene.getCamera().getRay(x, y);
            Color color = traceRay(ray, scene, MAX_DEPTH, 1);
            image.setPixelColor(x, y, color.getQColor());
        }
    });

    //Window Management
    QLabel label;
    label.setPixmap(QPixmap::fromImage(image));
    label.show();
    return QApplication::exec();
}