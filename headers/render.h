#pragma once

#include "scene.h"

struct Integrator {
    Integrator(Scene& scene);

    long long render(int samples);
    long long render1(int samples);
    long long render2(int samples);
    long long render3(int samples);
    long long render4(int samples);

    Scene scene;
    Texture outputImage;
};

struct ONB {
    Vector3f axis[3];

    ONB() {}

    ONB(const Vector3f& n) {
        axis[2] = Normalize(n);
        Vector3f a = (std::fabs(axis[2].x) > 0.9) ? Vector3f(0, 1, 0) : Vector3f(1, 0, 0);
        axis[1] = Normalize(Cross(axis[2], a));
        axis[0] = Cross(axis[2], axis[1]);
    }

    Vector3f world(const Vector3f& v) const {
        return v.x * axis[0] + v.y * axis[1] + v.z * axis[2];
    }

    Vector3f local(const Vector3f& v) const {
        return Vector3f(Dot(v, axis[0]), Dot(v, axis[1]), Dot(v, axis[2]));
    }
};