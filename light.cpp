#include "light.h"

Light::Light(LightType type, nlohmann::json config)
{
    switch (type)
    {
    case LightType::POINT_LIGHT:
        this->position = Vector3f(config["location"][0], config["location"][1], config["location"][2]);
        break;
    case LightType::DIRECTIONAL_LIGHT:
        this->direction = Vector3f(config["direction"][0], config["direction"][1], config["direction"][2]);
        break;
    case LightType::AREA_LIGHT:
        this->vx = Vector3f(config["vx"][0], config["vx"][1], config["vx"][2]);
        this->vy = Vector3f(config["vy"][0], config["vy"][1], config["vy"][2]);
        this->center = Vector3f(config["center"][0], config["center"][1], config["center"][2]);
        this->normal = Vector3f(config["normal"][0], config["normal"][1], config["normal"][2]);
        break;
    default:
        std::cout << "WARNING: Invalid light type detected";
        break;
    }

    this->radiance = Vector3f(config["radiance"][0], config["radiance"][1], config["radiance"][2]);
    this->type = type;
}

std::pair<Vector3f, LightSample> Light::sample(Interaction *si, int samples)
{
    LightSample ls;
    memset(&ls, 0, sizeof(ls));

    Vector3f radiance;
    switch (type)
    {
    case LightType::POINT_LIGHT:
        ls.wo = (position - si->p);
        ls.d = ls.wo.Length();
        ls.wo = Normalize(ls.wo);
        radiance = (1.f / (ls.d * ls.d)) * this->radiance;
        break;
    case LightType::DIRECTIONAL_LIGHT:
        ls.wo = Normalize(direction);
        ls.d = 1e10;
        radiance = this->radiance;
        break;
    case LightType::AREA_LIGHT:
        // Vector3f directionSum = Vector3f(0.0f, 0.0f, 0.0f);
        // Vector3f radianceSum = Vector3f(0.0f, 0.0f, 0.0f);
        // float distanceSum = 0.0f;
        // for (int s = 0; s < samples; ++s)
        // {
        // directionSum += direction;
        // distanceSum += distance;

        // float lengthVx = vx.Length();
        // float lengthVy = vy.Length();

        // // Compute the surface area of the area light source
        // float surfaceArea = lengthVx * lengthVy;
        // Vector3f radiance = this->radiance / surfaceArea;
        // radianceSum += radiance;
        //}
        // ls.wo = Normalize(directionSum / static_cast<float>(samples));
        // ls.d = distanceSum / static_cast<float>(samples);
        /*
        float u = next_float();
        float v = next_float();
        Vector3f sampledPoint = center + u * vx + v * vy;

        Vector3f direction = (sampledPoint - si->p);
        float distance = direction.Length();
        ls.wo = Normalize(direction);
        ls.d = distance;
        radiance = this->radiance;
        // radiance = radianceSum / static_cast<float>(samples);
        break;
        */
        Vector3f sample_point = center + 2 * (next_float() - 0.5) * vx + 2 * (next_float() - 0.5) * vy;
        float u = next_float();
        float v = next_float();
        //Vector3f sample_point = center + u * vx + v * vy;
        ls.wo = (sample_point - si->p);
        ls.d = ls.wo.Length();
        ls.wo = Normalize(ls.wo);
       radiance = this->radiance;

        break;
    }
    return {radiance, ls};
}

Interaction Light::intersectLight(Ray *ray)
{
    Interaction si;
    memset(&si, 0, sizeof(si));

    if (type == LightType::AREA_LIGHT)
    {
        if (Dot(ray->d, normal) == 0.f)
        {
            si.didIntersect = false;
            si.p = Vector3f(0, 0, 0);
            si.n = Vector3f(0, 0, 0);
            si.emissiveColor = Vector3f(0, 0, 0);
            return si;
        }
        Vector3f planeN = normal;
        Interaction one;
        one.didIntersect = false;
        float t;
        if (Dot(ray->d, planeN) < 0.f)
        {
            t = Dot(-ray->o + center, planeN) / Dot((ray->d), planeN);
            if (t >= 0.f)
            {
                one.didIntersect = true;
                one.t = t;
                one.n = normal;
                one.p = (ray->o) + ray->d * t;
            }
        }
        if(!one.didIntersect)
        {
            si.didIntersect = false;
            si.p = Vector3f(0, 0, 0);
            si.n = Vector3f(0, 0, 0);
            si.emissiveColor = Vector3f(0, 0, 0);
            return si;
        }
        if (one.didIntersect)
        {
            Vector3f intersection = ray->o + t * ray->d;
            //     // printf("%f %f %f\n",intersection[0],intersection[1],intersection[2]);

            float dx = Dot(intersection - center, vx);
            float dy = Dot(intersection - center, vy);

            if (fabs(dx)<=vx.LengthSquared() && fabs(dy)<=vy.LengthSquared())
            {
                si.didIntersect = true;
                 Vector3f sample_point = center + 2 * (next_float() - 0.5) * vx + 2 * (next_float() - 0.5) * vy;
                si.sampledPoint=sample_point;
                si.t = t;
                si.p = intersection;
                si.n = planeN;
                si.emissiveColor = radiance;
                return si;
                // printf("***%f %f %f\n",si.emissiveColor[0],si.emissiveColor[1],si.emissiveColor[2]);
            }
        }
    }
    else
    {
        si.didIntersect = false;
        si.sampledPoint=Vector3f(0,0,0);
        si.p = Vector3f(0, 0, 0);
        si.n = Vector3f(0, 0, 0);
        si.emissiveColor = Vector3f(0, 0, 0);
        return si;
    }

    return si;
}
