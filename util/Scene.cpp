//
// Created by nicolas on 14.03.26.
//

#include "Scene.h"

Scene::Scene(int width, int height, Color BACKGROUND_COLOR) : width(width), height(height), BACKGROUND_COLOR(BACKGROUND_COLOR){}

int Scene::getWidth() const {
    return width;
}

int Scene::getHeight() const {
    return height;
}

Color Scene::getBackgroundColor() const {
    return BACKGROUND_COLOR;
}

void Scene::addSphere(Sphere sphere) {
    spheres.emplace_back(sphere);
}

void Scene::addTriangle(Triangle triangle) {
    triangles.emplace_back(triangle);
}

void Scene::addLightSource(LightSource lightSource) {
    lightsources.emplace_back(lightSource);
}

std::pmr::vector<Sphere> Scene::getSpheres() const {
    return spheres;
}

std::pmr::vector<Triangle> Scene::getTriangles() const {
    return triangles;
}

std::pmr::vector<LightSource> Scene::getLightSources() const {
    return lightsources;
}