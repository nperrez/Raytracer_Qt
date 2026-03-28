//
// Created by nicolas on 26.03.26.
//

#ifndef RAYTRACER_QT_MATERIAL_H
#define RAYTRACER_QT_MATERIAL_H
#include "Color.h"

enum MaterialType {Lambert, Phong, BlinnPhong};

class Material {
private:
    MaterialType type;
    Color albedo;
    double shininess;
    double ks;
    double ka;
    double kd;

public:
    Material(MaterialType type, Color albedo) : type(type), albedo(albedo) {}
    Material(MaterialType type, Color albedo, double shininess, double ks, double ka, double kd) : type(type), albedo(albedo), shininess(shininess), ks(ks), ka(ka), kd(kd) {}

    MaterialType getType() const {
        return type;
    }

    Color getAlbedo() const {
        return albedo;
    }

    double getShininess() const {
        return shininess;
    }

    double getKs() const {
        return ks;
    }

    double getKa() const {
        return ka;
    }

    double getKd() const {
        return kd;
    }
};

#endif //RAYTRACER_QT_MATERIAL_H