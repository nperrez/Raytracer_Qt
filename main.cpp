#define QT_NO_KEYWORDS
#include <QApplication>
#include <QLabel>
#include <QImage>
#include <QTimer>
#include <queue>
#include <execution>
#include <random>
#include <atomic>
#include <thread>
#include <filesystem>

#include "3dObjects/Sphere.h"
#include "util/Color.h"
#include "3dObjects/LightSource.h"
#include "3dObjects/Triangle.h"
#include "util/Ray.h"
#include "util/Scene.h"
#include "util/Vector3d.h"

//Bildeigenschaften
constexpr int width = 1920;
constexpr int height = 1080;
QColor BACKGROUND_COLOR = Qt::black;
Color AMBIENT_COLOR = Color(0.1, 0.1, 0.1);
constexpr int MAX_DEPTH = 5;
constexpr int RANDOM_ITERATIONS_METAL = 64;
constexpr int RANDOM_ITERATIONS_GLASS = 64;
constexpr double EPSILON = 1e-6;

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

    Hit hit = scene.castRay(initialRay);
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
            const auto ray = Ray(hit.getPosition() + s * EPSILON, s);
            // Check ob Objekt blockiert ist
            for (auto &object: scene.getObjects()) {
                if (object->getMaterial().getType() == DIELECTRIC) continue;
                if (Hit hit2 = object->intersect(ray); hit2.getLambda() >= 0 && (lightSource.getPosition() - hit.getPosition()).getLength() > (hit2.getPosition() - hit.getPosition()).getLength()) {
                    blocked = true;
                    break;
                }
            }
            if (!blocked) {
                Color albedo = hit.getColor();
                //Schachbrettmuster
                if (hit.getMaterial().isCheckerboard()) {
                    int ix = (int)std::floor(hit.getPosition().getX() / 100.0);
                    int iz = (int)std::floor(hit.getPosition().getZ() / 100.0);
                    if (((ix + iz) % 2 + 2) % 2 != 0) albedo = hit.getMaterial().getAlbedo2();
                }
                diffuse = diffuse + lightSource.getColor() * albedo * hit.getMaterial().getDiffuseFact() * std::max(0.0, n * l);

                //Spekulare Lichtkomponente
                if (hit.getMaterial().getType() == SPECULAR) {
                    const Vector3d h = (l + v).normalize();
                    specular = specular + lightSource.getColor() * hit.getMaterial().getSpecularFact() * pow(std::max(0.0, n * h), hit.getMaterial().getShininess());
                }
            }
        }
    }

    //Berechnungen für Metall/Reflektierende Objekte
    if (hit.getMaterial().getType() == METAL) {
        if (hit.getMaterial().getGlossiness() == 0) {
            Ray nextRay = Ray(hit.getPosition() + hit.getNormal().normalize() * EPSILON, reflect(initialRay.getDirection(), hit.getNormal().normalize()));
            Hit reflectedHit = scene.castRay(nextRay);
            if (reflectedHit.getLambda() >= 0) {
                reflective = traceRay(nextRay, scene, depth -1, 1) * hit.getMaterial().getSpecular();
            }
        } else {
            Vector3d reflectiveDir = reflect(initialRay.getDirection(), hit.getNormal().normalize());
            Vector3d n = hit.getNormal().normalize();
            if (depth == 1) {
                Ray nextRay = Ray(hit.getPosition() + n * EPSILON, reflectiveDir);
                if (scene.castRay(nextRay).getLambda() >= 0)
                    reflective = traceRay(nextRay, scene, 0, 1) * hit.getMaterial().getSpecular();
            } else {

                //Berechnungen für matte Oberflächen
                double glossyR = 0, glossyG = 0, glossyB = 0;
                for (int i = 0; i < RANDOM_ITERATIONS_METAL; i++) {
                    Vector3d spreadDir = (reflectiveDir + randomVect() * hit.getMaterial().getGlossiness()).normalize();
                    if (spreadDir * n < 0) spreadDir = n * -(spreadDir * n);
                    Ray nextRay = Ray(hit.getPosition() + n * EPSILON, spreadDir);
                    if (scene.castRay(nextRay).getLambda() >= 0) {
                        Color reflectiveGlossy = traceRay(nextRay, scene, depth - 1, 1) * hit.getMaterial().getSpecular();
                        glossyR += reflectiveGlossy.getR();
                        glossyG += reflectiveGlossy.getG();
                        glossyB += reflectiveGlossy.getB();
                    }
                }
                reflective = Color(glossyR/RANDOM_ITERATIONS_METAL, glossyG/RANDOM_ITERATIONS_METAL, glossyB/RANDOM_ITERATIONS_METAL) * hit.getMaterial().getSpecular();
            }
        }
    }

    //Berechnungen für Glas/Refraktion
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
            //Berechnungen für perfektes Glas
            Ray reflectedRay = Ray(hit.getPosition() + n * EPSILON, reflectedDir);
            Color reflectedColor = traceRay(reflectedRay, scene, depth - 1, hit.getMaterial().getRefractiveIndex());
            if (!refractedDir) return reflectedColor;
            Ray refractedRay(hit.getPosition() + n * -EPSILON, *refractedDir);
            Color refractedColor = traceRay(refractedRay, scene, depth - 1, 1);
            if (!hit.isFrontFace()) {
                double r = exp(hit.getMaterial().getAbsorption().getR() * hit.getLambda() * -1);
                double g = exp(hit.getMaterial().getAbsorption().getG() * hit.getLambda() * -1);
                double b = exp(hit.getMaterial().getAbsorption().getB() * hit.getLambda() * -1);
                refractedColor = refractedColor * Color(r, g, b);
            }
            return reflectedColor * f + refractedColor * (1 - f);
        }

        //Berechnungen für frosted Glas
        if (depth == 1) {
            Ray reflectedRay = Ray(hit.getPosition() + n * EPSILON, reflectedDir);
            if (!refractedDir) return traceRay(reflectedRay, scene, 0, hit.getMaterial().getRefractiveIndex());
            Ray refractedRay(hit.getPosition() + n * -EPSILON, *refractedDir);
            return traceRay(reflectedRay, scene, 0, hit.getMaterial().getRefractiveIndex()) * f
                 + traceRay(refractedRay, scene, 0, 1) * (1 - f);
        }
        double glossyR = 0, glossyG = 0, glossyB = 0;
        for (int j = 0; j < RANDOM_ITERATIONS_GLASS; j++) {
            Color sampleColor;
            if (!refractedDir || randomFloat() < f) {
                Vector3d spreadDir = (reflectedDir + randomVect() * hit.getMaterial().getGlossiness()).normalize();
                if (spreadDir * n < 0) spreadDir = n * -(spreadDir * n);
                Ray sampleRay = Ray(hit.getPosition() + n * EPSILON, spreadDir);
                sampleColor = traceRay(sampleRay, scene, depth - 1, hit.getMaterial().getRefractiveIndex());
            } else {
                Vector3d spreadDir = ((*refractedDir) + randomVect() * hit.getMaterial().getGlossiness()).normalize();
                Ray sampleRay = Ray(hit.getPosition() + n * -EPSILON, spreadDir);
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
        return Color(glossyR / RANDOM_ITERATIONS_GLASS, glossyG / RANDOM_ITERATIONS_GLASS, glossyB / RANDOM_ITERATIONS_GLASS);
    }
    return AMBIENT_COLOR * hit.getMaterial().getAmbientFact() + diffuse + specular + reflective;
}

int main(int argc, char *argv[]) {

    //Scene setup
    const auto camera = Camera(Vector3d(-100, 180, -469), Vector3d(0, 200, 0), 1.65, width, height);
    auto scene = Scene(width, height, camera, BACKGROUND_COLOR);

    // Materialien
    Material wallBlue  = Material(Color(0.1, 0.1, 0.8), 0, 0, 0.2, 1.0);
    Material wallRed   = Material(Color(0.8, 0.1, 0.1), 0, 0.0, 0.2, 1.0);
    Material wallWhite = Material(Color(0.9, 0.9, 0.9), 0, 0.0, 0.2, 1.0);
    Material floorCheckerboard = Material(Color(0.9, 0.9, 0.9), 0, 0.0, 0.2, 1.0);
    floorCheckerboard.setCheckerboard(Color(0.1, 0.1, 0.1));
    Material wallMirror = Material(Color(0.4, 0.4, 0.5), 0);
    Material cube = Material(1.2, 0, Color(0.001, 0.01, 0.01));
    Material lucy = Material(Color(0.8, 0.2, 0.3), 100, 0.9, 0.2, 0.9);
    Material sphereFrosted = Material(1.1, 0.2, Color(0.006, 0.002, 0.001));
    Material sphereMetal = Material(Color(0, 0.8, 0.2), 0.1);
    Material sphereSpecular1 = Material(Color(0.8, 0.8, 0), 100, 1, 0.2, 0.9);
    Material sphereSpecular2 = Material(Color(0, 0.5, 0.9), 50, 1, 0.2, 0.9);
    Material sphereDiffuse = Material(Color(0.6, 0, 0.9), 0.2, 1);

    //Linke Wand (blau)
    scene.addTriangle(Vector3d(-800,-500,-500), Vector3d(-800, 500,-500), Vector3d(-800,-500, 500), wallBlue);
    scene.addTriangle(Vector3d(-800, 500,-500), Vector3d(-800, 500, 500), Vector3d(-800,-500, 500), wallBlue);
    //Rechte Wand (rot)
    scene.addTriangle(Vector3d( 800,-500,-500), Vector3d( 800,-500, 500), Vector3d( 800, 500,-500), wallRed);
    scene.addTriangle(Vector3d( 800,-500, 500), Vector3d( 800, 500, 500), Vector3d( 800, 500,-500), wallRed);
    //Rückwand (Spiegel)
    scene.addTriangle(Vector3d(-800,-500, 500), Vector3d(-800, 500, 500), Vector3d( 800,-500, 500), wallMirror);
    scene.addTriangle(Vector3d( 800,-500, 500), Vector3d(-800, 500, 500), Vector3d( 800, 500, 500), wallMirror);
    //Vorderwand (Spiegel)
    scene.addTriangle(Vector3d(-800, 500,-500), Vector3d(-800,-500,-500), Vector3d( 800,-500,-500), wallMirror);
    scene.addTriangle(Vector3d(-800, 500,-500), Vector3d( 800,-500,-500), Vector3d( 800, 500,-500), wallMirror);
    //Decke (weiss)
    scene.addTriangle(Vector3d(-800,-500,-500), Vector3d(-800,-500, 500), Vector3d( 800,-500,-500), wallWhite);
    scene.addTriangle(Vector3d( 800,-500,-500), Vector3d(-800,-500, 500), Vector3d( 800,-500, 500), wallWhite);
    //Boden (Schachbrett)
    scene.addTriangle(Vector3d(-800, 500,-500), Vector3d( 800, 500,-500), Vector3d(-800, 500, 500), floorCheckerboard);
    scene.addTriangle(Vector3d( 800, 500,-500), Vector3d( 800, 500, 500), Vector3d(-800, 500, 500), floorCheckerboard);

    //Lichtquellen
    scene.addLightSource(LightSource(Vector3d(   -420,  -400, 0), Color(0.4, 0.4, 0.4)));
    scene.addLightSource(LightSource(Vector3d(   410,  -400, 0), Color(0.7, 0.4, 0.4)));
    scene.addLightSource(LightSource(Vector3d(   0,  -400, -403), Color(0.4, 0.7, 0.4)));
    scene.addLightSource(LightSource(Vector3d(   0,  -400, 382), Color(0.4, 0.4, 0.7)));

    //Würfelpodest (rot - transparent)
    scene.addTriangle(Vector3d( -37, 298, -137), Vector3d( -37, 498, -137), Vector3d( 137, 298, -37), cube);
    scene.addTriangle(Vector3d( 137, 298,  -37), Vector3d( -37, 498, -137), Vector3d( 137, 498, -37), cube);

    scene.addTriangle(Vector3d( 137, 298,  -37), Vector3d( 137, 498,  -37), Vector3d(  37, 298, 137), cube);
    scene.addTriangle(Vector3d(  37, 298,  137), Vector3d( 137, 498,  -37), Vector3d(  37, 498, 137), cube);

    scene.addTriangle(Vector3d(  37, 298,  137), Vector3d(  37, 498,  137), Vector3d(-137, 298,  37), cube);
    scene.addTriangle(Vector3d(-137, 298,   37), Vector3d(  37, 498,  137), Vector3d(-137, 498,  37), cube);

    scene.addTriangle(Vector3d(-137, 298,   37), Vector3d(-137, 498,   37), Vector3d( -37, 298, -137), cube);
    scene.addTriangle(Vector3d( -37, 298, -137), Vector3d(-137, 498,   37), Vector3d( -37, 498, -137), cube);

    scene.addTriangle(Vector3d( -37, 298, -137), Vector3d( 137, 298,  -37), Vector3d(-137, 298,  37), cube);
    scene.addTriangle(Vector3d( 137, 298,  -37), Vector3d(  37, 298,  137), Vector3d(-137, 298,  37), cube);

    scene.addTriangle(Vector3d( -37, 498, -137), Vector3d(-137, 498,   37), Vector3d( 137, 498, -37), cube);
    scene.addTriangle(Vector3d(-137, 498,   37), Vector3d(  37, 498,  137), Vector3d( 137, 498, -37), cube);

    //Kugeln
    scene.addSphere(Vector3d(  -360,  415,  -90), 80, sphereFrosted);
    scene.addSphere(Vector3d(310, 400, 100), 100, sphereMetal);
    scene.addSphere(Vector3d(-300, 350, 300), 150, sphereSpecular1);
    scene.addSphere(Vector3d(450, 410, -150), 90, sphereDiffuse);
    scene.addSphere(Vector3d(300, 450, -200), 50, sphereSpecular2);

    //Lucy
    scene.addMesh(Mesh::fromObj("/home/nicolas/CLionProjects/Raytracer_Qt/objs/lucy.obj", 0.3, true, lucy, Vector3d(-220, 120, -20)));

    scene.build();

    //Fenstermanagement
    QApplication a(argc, argv);

    //Pixel Buffer
    std::vector<QRgb> pixels(width * height, qRgb(0, 0, 0));

    QLabel label;
    label.setPixmap(QPixmap(width, height));
    label.show();

    //Raytracing-Loop mit Multithreading
    std::atomic<bool> done{false};
    std::thread renderThread([&]() {
        std::vector<int> xs(width);
        std::iota(xs.begin(), xs.end(), 0);
        std::for_each(std::execution::par, xs.begin(), xs.end(), [&](const int x) {

            for (int y = 0; y < height; y++) {
                const Ray ray = scene.getCamera().getRay(x, y);
                Color color = traceRay(ray, scene, MAX_DEPTH, 1);
                pixels[y * width + x] = color.getQColor().rgba();
            }
        });
        done = true;
    });

    QTimer timer;
    QObject::connect(&timer, &QTimer::timeout, [&]() {
        QImage snap(reinterpret_cast<uchar *>(pixels.data()), width, height, QImage::Format_ARGB32);
        label.setPixmap(QPixmap::fromImage(snap.copy()));
        if (done) {
            timer.stop();
            std::filesystem::create_directories("images");
            int n = 1;
            while (std::filesystem::exists("images/render_" + std::to_string(n) + ".png")) ++n;
            snap.copy().save(QString::fromStdString("images/render_" + std::to_string(n) + ".png"));
        }
    });
    timer.start(100);

    int result = QApplication::exec();
    renderThread.join();
    return result;
}