//
// Created by nicolas on 02.04.26.
//

#ifndef RAYTRACER_QT_OBJECT_H
#define RAYTRACER_QT_OBJECT_H

class Object {
private:
    Material material;

public:
    Object(Material material) : material(material) {}
    Material getMaterial() const {
        return material;
    }
    Color getColor() const {
        return material.getAlbedo();
    }
};



#endif //RAYTRACER_QT_OBJECT_H