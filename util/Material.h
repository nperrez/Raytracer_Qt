//
// Created by nicolas on 26.03.26.
//

#ifndef RAYTRACER_QT_MATERIAL_H
#define RAYTRACER_QT_MATERIAL_H
#include "Color.h"


class Material {
private:
    Color albedo;

public:
    Material(Color albedo) : albedo(albedo) {}

    Color getAlbedo() const {
        return albedo;
    }
};

class Lambert : public Material {

public:
    Lambert(Color albedo) : Material(albedo) {}
};

class Phong : public Material {
private:
    double shininess;
    double ks;
    double ka;
    double kd;

public:
    Phong(const Color &albedo, const double shininess, const double ks, const double ka, const double kd) : Material(albedo), shininess(shininess), ks(ks), ka(ka), kd(kd) {}
    double getShininess() {
        return shininess;
    }
    double getKs() {
        return ks;
    }
    double getKa() {
        return ka;
    }
    double getKd() {
        return kd;
    }
};

class BlinnPhong : public Phong {
public:
    BlinnPhong(const Color &albedo, const double shininess, const double ks, const double ka, const double kd) : Phong(albedo, shininess, ks, ka, kd) {}
};


#endif //RAYTRACER_QT_MATERIAL_H