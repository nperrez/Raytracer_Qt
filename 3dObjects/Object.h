//
// Created by nicolas on 02.04.26.
//

#ifndef RAYTRACER_QT_OBJECT_H
#define RAYTRACER_QT_OBJECT_H

class Object {
private:
    Material material;

public:
    Object(const Material &material) : material(material) {}
    virtual ~Object() = default;
    Material getMaterial() const {
        return material;
    }
    Color getColor() const {
        return material.getAlbedo();
    }
    virtual Hit intersect(Ray ray) const = 0;
};



#endif //RAYTRACER_QT_OBJECT_H