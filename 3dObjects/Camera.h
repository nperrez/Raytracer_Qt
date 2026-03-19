//
// Created by nicolas on 19.03.26.
//

#ifndef RAYTRACER_QT_CAMERA_H
#define RAYTRACER_QT_CAMERA_H

#include <../../../util/Vector3d.h>
#include <../../../util/Ray.h>


class Camera {

private:
    Vector3d position, forward, right, up;
    double fov, aspectRatio, halfHeight, halfWidth;
    int width, height;

public:
    Camera(Vector3d position, Vector3d lookAt, double fov, int width, int height);

    Vector3d getForward() const;
    Vector3d getRight() const;
    Vector3d getUp() const;
    double getFOV() const;
    double getAspectRatio() const;
    double getHalfHeight() const;
    double getHalfWidth() const;
    Ray getRay(double x, double y) const;
};


#endif //RAYTRACER_QT_CAMERA_H