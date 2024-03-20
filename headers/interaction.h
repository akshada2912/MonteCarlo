#pragma once

#include "vec.h"

// Forward declaration of BSDF class
class BSDF;

struct Interaction
{
    // Position of interaction
    Vector3f p;
    // Normal of the surface at interaction
    Vector3f n;
    // The uv co-ordinates at the intersection point
    Vector2f uv;
    // The viewing direction in local shading frame
    Vector3f wi;
    // Distance of intersection point from origin of the ray
    float t = 1e30f;
    // Used for light intersection, holds the radiance emitted by the emitter.
    Vector3f emissiveColor = Vector3f(0.f, 0.f, 0.f);
    Vector3f sampledPoint;
    // BSDF at the shading point
    BSDF *bsdf;
    // Vectors defining the orthonormal basis
    Vector3f a, b, c;

    bool didIntersect = false;

    Vector3f toWorld(Vector3f w)
    {
        return Vector3f(a.x * w.x + b.x * w.y + c.x * w.z, a.y * w.x + b.y * w.y + c.y * w.z, a.z * w.x + b.z * w.y + c.z * w.z);
    }

    Vector3f toLocal(Vector3f w)
    {
        return Vector3f(a.x * w.x + a.y * w.y + a.z * w.z, b.x * w.x + b.y * w.y + b.y * w.z, c.x * w.x + c.y * w.y + c.z * w.z);
    }

    // Vector3f toWorld(Vector3f w) {
    //     // TODO: Implement this
    //     Vector3f ans = Vector3f(0,0,0);
    //     ans.x = w.x*this->a.x + w.y*this->a.y + w.z*this->a.z;
    //     ans.y = w.x*this->b.x + w.y*this->b.y + w.z*this->b.z;
    //     ans.z = w.x*this->c.x + w.y*this->c.y + w.z*this->c.z;

    //     return ans;
    // }

    // Vector3f toLocal(Vector3f w) {
    //     Vector3f ans = Vector3f(0,0,0);
    //     ans.x = w.x * this->a.x + w.y * this->b.x + w.z * this->c.x;
    //     ans.y = w.x * this->a.y + w.y * this->b.y + w.z * this->c.y;
    //     ans.z = w.x * this->a.z + w.y * this->b.z + w.z * this->c.z;

    //     // TODO: Implement this
    //     return ans;
    // }

    // Vector3f toWorld(Vector3f w) {
    //     // TODO: Implement this
    //      return w.x * b + w.y * c + w.z * a;
    // }

    // Vector3f toLocal(Vector3f w) {
    //     // TODO: Implement this
    //     return Vector3f(Dot(w, b), Dot(w, c), Dot(w, a));
    // }
};