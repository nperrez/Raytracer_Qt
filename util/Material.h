//
// Created by nicolas on 26.03.26.
//

#ifndef RAYTRACER_QT_MATERIAL_H
#define RAYTRACER_QT_MATERIAL_H
#include "Color.h"

enum MaterialType {DIFFUSE, SPECULAR, METAL, DIELECTRIC};

class Material {
private:
    MaterialType type;
    Color albedo;
    Color specular;
    Color absorption;
    double shininess;
    double glossiness;
    double specularFact;
    double ambientFact;
    double diffuseFact;
    double refractiveIndex;

public:
    Material(Color albedo) : albedo(albedo) {
        this->shininess = 0;
        this->specularFact = 1;
        this->ambientFact = 1;
        this->diffuseFact = 1;
        this->type = DIFFUSE;
        this->refractiveIndex = 0;
        this->glossiness = 0;
    }

    Material(Color albedo, double ka, double kd) :albedo(albedo), ambientFact(ka), diffuseFact(kd) {
        this->type = DIFFUSE;
        this->shininess = 0;
        this->specularFact = 0;
        this->glossiness = 0;
        this->refractiveIndex = 0;
    }

    Material(Color albedo, double shininess, double ks, double ka, double kd) : albedo(albedo), shininess(shininess), specularFact(ks), ambientFact(ka), diffuseFact(kd) {
        this->type = SPECULAR;
        this->glossiness = 0;
        this->refractiveIndex = 0;
    }

    Material(Color specular, double glossiness) : specular(specular), glossiness(glossiness) {
        this->type = METAL;
        this->shininess = 0;
        this->specularFact = 0;
        this->ambientFact = 0;
        this->diffuseFact = 0;
        this->refractiveIndex = 0;
    }

    Material(double refractiveIndex, Color absorption) : absorption(absorption), refractiveIndex(refractiveIndex) {
        this->type = DIELECTRIC;
        this->shininess = 0;
        this->specularFact = 0;
        this->ambientFact = 0;
        this->diffuseFact = 0;
        this->glossiness = 0;
    }

    MaterialType getType() const {
        return type;
    }

    Color getAlbedo() const {
        return albedo;
    }

    Color getSpecular() const {
        return specular;
    }

    double getShininess() const {
        return shininess;
    }

    double getGlossiness() const {
        return glossiness;
    }

    double getSpecularFact() const {
        return specularFact;
    }

    double getAmbientFact() const {
        return ambientFact;
    }

    double getDiffuseFact() const {
        return diffuseFact;
    }

    double getRefractiveIndex() const {
        return refractiveIndex;
    }
};

#endif //RAYTRACER_QT_MATERIAL_H