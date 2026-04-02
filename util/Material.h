//
// Created by nicolas on 26.03.26.
//

#ifndef RAYTRACER_QT_MATERIAL_H
#define RAYTRACER_QT_MATERIAL_H
#include "Color.h"

class Material {
private:
    Color albedo;
    Color specular;
    double shininess;
    double glossiness;
    double specularFact;
    double ambientFact;
    double diffuseFact;
    double reflectionFact;
    double refractionFact;

public:
    Material(Color albedo) : albedo(albedo) {
        this->shininess = 0;
        this->specularFact = 1;
        this->ambientFact = 1;
        this->diffuseFact = 1;
    }
    Material(Color albedo, double shininess, double ks, double ka, double kd) : albedo(albedo), shininess(shininess), specularFact(ks), ambientFact(ka), diffuseFact(kd) {
        this->glossiness = 0;
        this->reflectionFact = 0;
        this->refractionFact = 0;
    }

    Material(Color albedo, Color specular, double shininess, double glossiness, double ks, double ka, double kd, double krl) : albedo(albedo), specular(specular), shininess(shininess), glossiness(glossiness), specularFact(ks), ambientFact(ka), diffuseFact(kd), reflectionFact(krl) {
        this->refractionFact = 0;
    }

    Material(Color specular, double glossiness) : specular(specular), glossiness(glossiness) {
        this->shininess = 0;
        this->specularFact = 0;
        this->ambientFact = 0;
        this->diffuseFact = 0;
        this->reflectionFact = 1;
        this->refractionFact = 0;
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

    double getReflectionFact() const {
        return reflectionFact;
    }
};

#endif //RAYTRACER_QT_MATERIAL_H