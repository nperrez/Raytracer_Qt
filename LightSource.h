//
// Created by nicolas on 05.03.26.
//

#ifndef RAYTRACER_QT_LIGHTSOURCE_H
#define RAYTRACER_QT_LIGHTSOURCE_H
#include "Color.h"
#include "Vector3d.h"


class LightSource {

private:
    Vector3d position;
    Color color;

public:
    LightSource(Vector3d position, Color color);
    Vector3d getPosition();
    Color getColor();

};


#endif //RAYTRACER_QT_LIGHTSOURCE_H