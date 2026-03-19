//
// Created by nicolas on 14.03.26.
//

#ifndef RAYTRACER_QT_SCENE_H
#define RAYTRACER_QT_SCENE_H
#include <format>
#include <vector>
#include <../../../3dObjects/Sphere.h>
#include <../../../3dObjects/Triangle.h>
#include <../../../3dObjects/Camera.h>
#include "../3dObjects/LightSource.h"


class Scene {

private:
    std::pmr::vector<Sphere> spheres;
    std::pmr::vector<Triangle> triangles;
    std::pmr::vector<LightSource> lightsources;
    Camera camera;
    int width, height;
    Color BACKGROUND_COLOR;

public:
    Scene(int width, int height, Camera camera, Color BACKGROUND_COLOR);
    void addSphere(Sphere sphere);
    void addTriangle(Triangle triangle);
    void addLightSource(LightSource lightSource);
    int getWidth() const;
    int getHeight() const;
    Color getBackgroundColor() const;
    std::pmr::vector<Sphere> getSpheres() const;
    std::pmr::vector<Triangle> getTriangles() const;
    std::pmr::vector<LightSource> getLightSources() const;
    Camera getCamera() const;

};


#endif //RAYTRACER_QT_SCENE_H