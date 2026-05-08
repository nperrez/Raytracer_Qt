//
// Created by nicolas on 08.05.26.
//

#ifndef RAYTRACER_QT_AABB_H
#define RAYTRACER_QT_AABB_H

#include <algorithm>
#include <limits>
#include "Vector3d.h"
#include "Ray.h"

class AABB {
public:
    Vector3d min;
    Vector3d max;

    // Default: inverted-infinite box — expand() into it to build from scratch
    AABB()
        : min( std::numeric_limits<double>::infinity(),
               std::numeric_limits<double>::infinity(),
               std::numeric_limits<double>::infinity())
        , max(-std::numeric_limits<double>::infinity(),
              -std::numeric_limits<double>::infinity(),
              -std::numeric_limits<double>::infinity())
    {}

    AABB(const Vector3d &min, const Vector3d &max) : min(min), max(max) {}

    void expand(const Vector3d &p) {
        min = Vector3d(std::min(min.getX(), p.getX()),
                       std::min(min.getY(), p.getY()),
                       std::min(min.getZ(), p.getZ()));
        max = Vector3d(std::max(max.getX(), p.getX()),
                       std::max(max.getY(), p.getY()),
                       std::max(max.getZ(), p.getZ()));
    }

    void expand(const AABB &other) {
        expand(other.min);
        expand(other.max);
    }

    // Slab-method ray-box intersection. Returns true if the ray hits
    // the box in the interval (tMin, tMax).
    bool hit(Ray ray, double tMin, double tMax) const {
        auto check = [&](double orig, double dir, double bMin, double bMax) -> bool {
            if (std::abs(dir) < 1e-12) {
                return orig >= bMin && orig <= bMax;
            }
            double invD = 1.0 / dir;
            double t0 = (bMin - orig) * invD;
            double t1 = (bMax - orig) * invD;
            if (t0 > t1) std::swap(t0, t1);
            tMin = std::max(tMin, t0);
            tMax = std::min(tMax, t1);
            return tMin <= tMax;
        };

        Vector3d o = ray.getLocation();
        Vector3d d = ray.getDirection();
        return check(o.getX(), d.getX(), min.getX(), max.getX())
            && check(o.getY(), d.getY(), min.getY(), max.getY())
            && check(o.getZ(), d.getZ(), min.getZ(), max.getZ());
    }
};

#endif //RAYTRACER_QT_AABB_H
