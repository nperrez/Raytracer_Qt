//
// Created by Nicolas Perrez on 26/02/2026.
//

#ifndef RAYTRACER_QT_VECTOR3D_H
#define RAYTRACER_QT_VECTOR3D_H


class Vector3d {

private:
    double x, y, z;

public:
    Vector3d();
    Vector3d(double x, double y, double z);

    double getX() const;
    double getY() const;
    double getZ() const;
    double getLength() const;

    Vector3d operator+(const Vector3d &) const;
    Vector3d operator-(const Vector3d &) const;
    Vector3d operator*(double scalar) const;
    double operator*(const Vector3d &) const;
    Vector3d operator/(double scalar) const;
};


#endif //RAYTRACER_QT_VECTOR3D_H