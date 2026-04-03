//
// Created by nicolas on 05.03.26.
//

#ifndef RAYTRACER_QT_HIT_H
#define RAYTRACER_QT_HIT_H
#include "Color.h"
#include "Material.h"
#include "Vector3d.h"


class Hit {

private:
    double lambda;
    double u;
    double v;
    Vector3d position;
    Vector3d normal;
    Material material;
    bool frontFace;

public:
    Hit(double lambda, Vector3d position, Vector3d normal, const Material &material);
    Hit();
    double getLambda();
    Vector3d getPosition();
    Vector3d getNormal();
    Material getMaterial();
    Color getColor();
    void setU(double u);
    void setV(double v);
    double getU();
    double getV();
    void setFrontFace(const Vector3d& rayDirection, const Vector3d& outwardNormal);
    bool isFrontFace() const;
};


#endif //RAYTRACER_QT_HIT_H