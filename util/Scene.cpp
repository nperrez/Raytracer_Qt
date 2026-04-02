//
// Created by nicolas on 14.03.26.
//

#include "Scene.h"
#include <memory>
#include <memory_resource>

Scene::Scene(int width, int height, Camera camera, Color BACKGROUND_COLOR) : width(width), height(height), camera(camera), BACKGROUND_COLOR(BACKGROUND_COLOR){}

int Scene::getWidth() const {
    return width;
}

int Scene::getHeight() const {
    return height;
}

Color Scene::getBackgroundColor() const {
    return BACKGROUND_COLOR;
}

void Scene::addSphere(const Vector3d &center, const double radius, const Material &material) {
    objects.push_back(std::make_unique<Sphere>(center, radius, material));
}

void Scene::addTriangle(const Vector3d &a, const Vector3d &b, const Vector3d &c, const Material &material) {
    objects.push_back(std::make_unique<Triangle>(a, b, c, material));
}

void Scene::addLightSource(LightSource lightSource) {
    lightsources.emplace_back(lightSource);
}

const std::pmr::vector<std::unique_ptr<Object>>& Scene::getObjects() const {
    return objects;
}

std::pmr::vector<LightSource> Scene::getLightSources() const {
    return lightsources;
}

Camera Scene::getCamera() const {
    return camera;
}