//
// Created by nicolas on 26.03.26.
//

#ifndef RAYTRACER_QT_MATERIAL_H
#define RAYTRACER_QT_MATERIAL_H
#include "Color.h"
#include <cmath>
#include <algorithm>

enum MaterialType {DIFFUSE, SPECULAR, METAL, DIELECTRIC, MESH};

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
    Material() {
        this->type = MESH;
        this->shininess = 0;
        this->specularFact = 0;
        this->ambientFact = 0;
        this->diffuseFact = 0;
        this->refractiveIndex = 0;
        this->glossiness = 0;
    }

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

    Material(double refractiveIndex, double glossiness, const Color &absorption) : absorption(absorption), refractiveIndex(refractiveIndex), glossiness(glossiness) {
        this->type = DIELECTRIC;
        this->shininess = 0;
        this->specularFact = 0;
        this->ambientFact = 0;
        this->diffuseFact = 0;
    }

    // Full .mtl constructor. illum selects the material type:
    //   0-1 → DIFFUSE, 2 → SPECULAR, 3/5/8 → METAL, 4/6/7/9 → DIELECTRIC
    // roughness has no .mtl equivalent; pass 0 for sharp glass/metal.
    Material(const Color &Ka, const Color &Kd, const Color &Ks, const Color &Tf,
             double Ns, double Ni, double d, double roughness, int illum) {
        this->shininess = Ns;
        this->refractiveIndex = Ni;
        this->glossiness = roughness;

        if (illum <= 1) {
            this->type = DIFFUSE;
            this->albedo = Kd;
            this->specular = Color(0, 0, 0);
            this->ambientFact = (Ka.getR() + Ka.getG() + Ka.getB()) / 3.0;
            this->diffuseFact = d;
            this->specularFact = 0;
            this->absorption = Color(0, 0, 0);
        } else if (illum == 2) {
            this->type = SPECULAR;
            this->albedo = Kd;
            this->specular = Ks;
            this->ambientFact = (Ka.getR() + Ka.getG() + Ka.getB()) / 3.0;
            this->diffuseFact = d;
            this->specularFact = (Ks.getR() + Ks.getG() + Ks.getB()) / 3.0;
            this->absorption = Color(0, 0, 0);
        } else if (illum == 3 || illum == 5 || illum == 8) {
            this->type = METAL;
            this->albedo = Kd;
            this->specular = Ks;
            this->ambientFact = 0;
            this->diffuseFact = 0;
            this->specularFact = 0;
            this->absorption = Color(0, 0, 0);
        } else {
            // illum 4, 6, 7, 9 → DIELECTRIC
            this->type = DIELECTRIC;
            this->albedo = Kd;
            this->specular = Ks;
            this->ambientFact = 0;
            this->diffuseFact = 0;
            this->specularFact = 0;
            // Convert Tf (transmission filter) to Beer's law absorption: a = -log(Tf).
            // Tf=(1,1,1) → absorption=(0,0,0) (clear). Tf=(1,0,0) → only red passes.
            auto toAbs = [](double t) { return -std::log(std::max(t, 1e-9)); };
            this->absorption = Color(toAbs(Tf.getR()), toAbs(Tf.getG()), toAbs(Tf.getB()));
        }
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

    Color getAbsorption() const {
        return absorption;
    }
};

#endif //RAYTRACER_QT_MATERIAL_H