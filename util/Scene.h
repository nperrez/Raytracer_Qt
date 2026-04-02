//
// Created by nicolas on 14.03.26.
//

#ifndef RAYTRACER_QT_SCENE_H
#define RAYTRACER_QT_SCENE_H
#include <format>
#include <vector>
#include <memory>
#include <memory_resource>
#include <../../../3dObjects/Sphere.h>
#include <../../../3dObjects/Triangle.h>
#include <../../../3dObjects/Camera.h>
#include "../3dObjects/LightSource.h"


class Scene {

private:
    std::pmr::vector<Sphere> spheres;
    std::pmr::vector<Triangle> triangles;
    std::pmr::vector<std::unique_ptr<Object>> objects;
    std::pmr::vector<LightSource> lightsources;

    Camera camera;
    int width, height;
    Color BACKGROUND_COLOR;

public:
    Scene(int width, int height, Camera camera, Color BACKGROUND_COLOR);
    void addSphere(const Vector3d &center, double radius, const Material &material);
    void addTriangle(const Vector3d &a, const Vector3d &b, const Vector3d &c, const Material &material);
    void addLightSource(LightSource lightSource);
    int getWidth() const;
    int getHeight() const;
    Color getBackgroundColor() const;
    std::pmr::vector<Sphere> getSpheres() const;
    std::pmr::vector<Triangle> getTriangles() const;
    const std::pmr::vector<std::unique_ptr<Object>>& getObjects() const;
    std::pmr::vector<LightSource> getLightSources() const;
    Camera getCamera() const;

};


#endif //RAYTRACER_QT_SCENE_H