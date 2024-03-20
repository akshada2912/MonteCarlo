#include "render.h"
#include "random"

Integrator::Integrator(Scene &scene)
{
    this->scene = scene;
    this->outputImage.allocate(TextureType::UNSIGNED_INTEGER_ALPHA, this->scene.imageResolution);
}

long long Integrator::render(int samples)
{
    auto startTime = std::chrono::high_resolution_clock::now();
    for (int x = 0; x < this->scene.imageResolution.x; x++)
    {
        for (int y = 0; y < this->scene.imageResolution.y; y++)
        {

            Vector3f color(0, 0, 0);
            for (int s = 0; s < samples; s++)
            {
                Vector3f result(0, 0, 0);
                float epsilon1 = next_float();
                float epsilon2 = next_float();
                Ray cameraRay = this->scene.camera.generateRay(x, y, epsilon1, epsilon2);
                Interaction si = this->scene.rayIntersect(cameraRay);
                if (si.didIntersect)
                {

                    Vector3f radiance;
                    LightSample ls;
                    for (Light &light : this->scene.lights)
                    {
                        std::tie(radiance, ls) = light.sample(&si, samples);

                        Ray shadowRay(si.p + 1e-3f * si.n, ls.wo);
                        Interaction siShadow = this->scene.rayIntersect(shadowRay);
                        // Interaction si2 = this->scene.rayEmitterIntersect(cameraRay);
                        if (!siShadow.didIntersect || siShadow.t > ls.d)
                        // if (!siShadow.didIntersect || siShadow.t > ls.d)
                        {

                            // radiance += si2.emissiveColor;
                            result += si.bsdf->eval(&si, si.toLocal(ls.wo)) * radiance * std::abs(Dot(si.n, ls.wo));
                            //  result+=si2.emissiveColor;
                            // result += radiance;
                        }
                    }
                    // result -= 0.81 * (si.bsdf->eval(&si, si.toLocal(ls.wo)) * radiance * std::abs(Dot(si.n, ls.wo)));
                    color += result;
                }
            }
            color = color / samples;
            this->outputImage.writePixelColor(color, x, y);
        }
    }
    auto finishTime = std::chrono::high_resolution_clock::now();

    return std::chrono::duration_cast<std::chrono::microseconds>(finishTime - startTime).count();
}

long long Integrator::render1(int samples)
{
    auto startTime = std::chrono::high_resolution_clock::now();
    for (int x = 0; x < this->scene.imageResolution.x; x++)
    {
        for (int y = 0; y < this->scene.imageResolution.y; y++)
        {

            Vector3f color(0, 0, 0);
            for (int s = 0; s < samples; s++)
            {
                Vector3f result(0, 0, 0);
                float epsilon1 = next_float();
                float epsilon2 = next_float();
                Ray cameraRay = this->scene.camera.generateRay(x, y, epsilon1, epsilon2);
                Interaction si = this->scene.rayIntersect(cameraRay);
                if (si.didIntersect)
                {

                    Vector3f radiance;
                    LightSample ls;
                    for (Light &light : this->scene.lights)
                    {
                        std::tie(radiance, ls) = light.sample(&si, samples);

                        Ray shadowRay(si.p + 1e-3f * si.n, ls.wo);
                        Interaction siShadow = this->scene.rayIntersect(shadowRay);
                        if (!siShadow.didIntersect || siShadow.t > ls.d)
                        // if (!siShadow.didIntersect || siShadow.t > ls.d)
                        {

                            // radiance += si2.emissiveColor;
                            result += si.bsdf->eval(&si, si.toLocal(ls.wo)) * radiance * std::abs(Dot(si.n, ls.wo));
                            //  result+=si2.emissiveColor;
                            // result += radiance;
                        }
                    }
                    color += result;
                }

                Interaction si2 = this->scene.rayEmitterIntersect(cameraRay);
                if (si2.didIntersect)
                    color += si2.emissiveColor;
            }
            color = color / samples;
            this->outputImage.writePixelColor(color, x, y);
        }
    }
    auto finishTime = std::chrono::high_resolution_clock::now();

    return std::chrono::duration_cast<std::chrono::microseconds>(finishTime - startTime).count();
}

Vector3f SampleUniformHemisphere2(float u1, float u2)
{
    float theta = acos(u1);
    float phi = 2 * M_PI * u2;
    return Vector3f(sin(theta) * cos(phi), sin(theta) * sin(phi), cos(theta));

    // float z = u1;
    // float r = std::sqrt(std::max((float)0, (float)1. - z * z));
    // float phi = 2 * M_PI * u2;
    // return Vector3f(r * std::cos(phi), r * std::sin(phi), z);
}

long long Integrator::render2(int samples)
{
    auto startTime = std::chrono::high_resolution_clock::now();
    for (int x = 0; x < this->scene.imageResolution.x; x++)
    {
        for (int y = 0; y < this->scene.imageResolution.y; y++)
        {
            Vector3f result(0, 0, 0);
            for (int s = 0; s < samples; s++)
            {
                Ray cameraRay = this->scene.camera.generateRay(x, y, 0.5, 0.5);
                Interaction si = this->scene.rayIntersect(cameraRay);
                Interaction si2 = this->scene.rayEmitterIntersect(cameraRay);

                if (si.didIntersect && (si.t < si2.t || !si2.didIntersect))
                {
                    Vector3f radiance;
                    LightSample ls;
                    for (Light &light : this->scene.lights)
                    {
                        if (light.type != AREA_LIGHT)
                        {
                            std::tie(radiance, ls) = light.sample(&si, samples);

                            Ray shadowRay(si.p + 1e-3f * si.n, ls.wo);
                            Interaction siShadow = this->scene.rayIntersect(shadowRay);

                            if (!siShadow.didIntersect || siShadow.t > ls.d)
                            {
                                result += si.bsdf->eval(&si, si.toLocal(ls.wo)) * radiance * std::abs(Dot(si.n, ls.wo));
                            }
                        }
                    }
                    Vector3f color(0, 0, 0);
                    // for (int s = 0; s < samples; s++)
                    // {
                        Vector3f dir_local = SampleUniformHemisphere2(next_float(), next_float());
                        Vector3f dir_global = si.toWorld(dir_local);

                        Ray shadowRay(si.p + 1e-3f * si.n, dir_global);
                        Interaction siShadow = this->scene.rayIntersect(shadowRay);
                        Interaction light_intersection = this->scene.rayEmitterIntersect(shadowRay);

                        if ((!siShadow.didIntersect || siShadow.t > light_intersection.t) && light_intersection.didIntersect)
                        {
                            color += si.bsdf->eval(&si, dir_local) * light_intersection.emissiveColor * std::abs(Dot(si.n, dir_global));
                            // color += si.bsdf->eval(&si, dir_local) * light_intersection.emissiveColor;
                        }
                    //}
                    color *= 2 * M_PI;
                    // color *= M_PI;
                    result += color / samples;
                }
                if (si2.didIntersect)
                {
                    // ifthe *closest* object we are looking at is NOT a surface, it is an area light
                    // if (si2.didIntersect)
                    result += si2.emissiveColor;
                }
            }

            this->outputImage.writePixelColor(result , x, y);
        }
    }
    auto finishTime = std::chrono::high_resolution_clock::now();

    return std::chrono::duration_cast<std::chrono::microseconds>(finishTime - startTime).count();
}

Vector2f ConcentricSampleDisk(float u1, float u2)
{
    Vector2f uOffset = 2.f * Vector2f(u1, u2) - Vector2f(1, 1);
    if (uOffset.x == 0 && uOffset.y == 0)
        return Vector2f(0, 0);
    float theta,
        r;
    if (std::abs(uOffset.x) > std::abs(uOffset.y))
    {
        r = uOffset.x;
        theta = M_PI / 4 * (uOffset.y / uOffset.x);
    }
    else
    {
        r = uOffset.y;
        theta = M_PI / 2 - M_PI / 4 * (uOffset.x / uOffset.y);
    }
    return r * Vector2f(std::cos(theta), std::sin(theta));
}

Vector3f SampleCosineHemisphere(float u1, float u2)
{
    Vector2f d = ConcentricSampleDisk(u1, u2);
    float z = std::sqrt(std::max((float)0, 1 - d.x * d.x - d.y * d.y));
    return Vector3f(d.x, d.y, z);
}

long long Integrator::render3(int samples)
{
    auto startTime = std::chrono::high_resolution_clock::now();
    for (int x = 0; x < this->scene.imageResolution.x; x++)
    {
        for (int y = 0; y < this->scene.imageResolution.y; y++)
        {
            Vector3f result(0, 0, 0);
            for (int s = 0; s < samples; s++)
            {
                Ray cameraRay = this->scene.camera.generateRay(x, y, 0.5, 0.5);
                Interaction si = this->scene.rayIntersect(cameraRay);
                Interaction si2 = this->scene.rayEmitterIntersect(cameraRay);

                if (si.didIntersect && (si.t < si2.t || !si2.didIntersect))
                {
                    Vector3f radiance;
                    LightSample ls;
                    for (Light &light : this->scene.lights)
                    {
                        if (light.type != AREA_LIGHT)
                        {
                            std::tie(radiance, ls) = light.sample(&si, samples);

                            Ray shadowRay(si.p + 1e-3f * si.n, ls.wo);
                            Interaction siShadow = this->scene.rayIntersect(shadowRay);

                            if (!siShadow.didIntersect || siShadow.t > ls.d)
                            {
                                result += si.bsdf->eval(&si, si.toLocal(ls.wo)) * radiance * std::abs(Dot(si.n, ls.wo));
                            }
                        }
                    }
                    Vector3f color(0, 0, 0);
                        Vector3f dir_local = SampleCosineHemisphere(next_float(), next_float());
                        Vector3f dir_global = si.toWorld(dir_local);

                        Ray shadowRay(si.p + 1e-3f * si.n, dir_global);
                        Interaction siShadow = this->scene.rayIntersect(shadowRay);
                        Interaction light_intersection = this->scene.rayEmitterIntersect(shadowRay);

                        if ((!siShadow.didIntersect || siShadow.t > light_intersection.t) && light_intersection.didIntersect)
                        {
                            // color += si.bsdf->eval(&si, dir_local) * light_intersection.emissiveColor * std::abs(Dot(si.n, dir_global));
                            color += si.bsdf->eval(&si, dir_local) * light_intersection.emissiveColor;
                        }
                    // color *= 2 * M_PI;
                    color *= M_PI;
                    result += color / samples;
                }
                if (si2.didIntersect)
                {
                    // ifthe *closest* object we are looking at is NOT a surface, it is an area light
                    // if (si2.didIntersect)
                    result += si2.emissiveColor;
                }
            }

            this->outputImage.writePixelColor(result, x, y);
        }
    }
    auto finishTime = std::chrono::high_resolution_clock::now();

    return std::chrono::duration_cast<std::chrono::microseconds>(finishTime - startTime).count();
}

int next_int(int min, int max)
{
    // Returns a random integer in the range [min, max].
    return min + rand() % (max - min + 1);
}



long long Integrator::render4(int samples)
{
    auto startTime = std::chrono::high_resolution_clock::now();
    for (int x = 0; x < this->scene.imageResolution.x; x++)
    {
        for (int y = 0; y < this->scene.imageResolution.y; y++)
        {
            Vector3f result(0, 0, 0);
            for (int r = 0; r < samples; r++)
            {
                int spp_copy = samples;
                Ray cameraRay = this->scene.camera.generateRay(x, y,0.5,0.5);
                Interaction si = this->scene.rayIntersect(cameraRay);
                Interaction si2 = this->scene.rayEmitterIntersect(cameraRay);

                if (si.didIntersect && (si.t < si2.t || !si2.didIntersect))
                {
                    Vector3f radiance;
                    LightSample ls;
                    for (Light &light : this->scene.lights)
                    {
                        if (light.type != AREA_LIGHT)
                        {
                            std::tie(radiance, ls) = light.sample(&si,samples);

                            Ray shadowRay(si.p + 1e-3f * si.n, ls.wo);
                            Interaction siShadow = this->scene.rayIntersect(shadowRay);

                            if (!siShadow.didIntersect || siShadow.t > ls.d)
                            {
                                result += si.bsdf->eval(&si, si.toLocal(ls.wo)) * radiance * std::abs(Dot(si.n, ls.wo));
                            }
                        }
                    }
                    int num_lights = this->scene.lights.size();
                    Vector3f color(0, 0, 0);
                    // for(int s=0;s<samples;s++)
                    // {
                        for (Light &light : this->scene.lights)
                        {
                            if (light.type == AREA_LIGHT)
                            {
                                std::tie(radiance, ls) = light.sample(&si,samples);

                                Ray shadowRay(si.p + 1e-3f * si.n, ls.wo);
                                Interaction siShadow = this->scene.rayIntersect(shadowRay);
                                float area = 4 * Cross(light.vx, light.vy).Length();
                                //and if the angle between the light direction ls.wo and the light's normal light.normal is less than or equal to 90 degrees (i.e., the light is illuminating the surface).
                                if ((!siShadow.didIntersect || siShadow.t > ls.d) && Dot(ls.wo, light.normal) <= 0)
                                {
                                    //cosine of the angle between the surface normal si.n and the direction to the light source ls.wo
                                    // cosine of the angle between the light's normal (which might be different from the surface normal, depending on the light source) and the direction to the light source ls.wo
                                    // inverse square law for light falloff. It divides the area of the light source by the square of the distance between the surface point and the light source. This accounts for the fact that light spreads out as it travels, resulting in a decrease in intensity with distance.
                                    color += si.bsdf->eval(&si, si.toLocal(ls.wo)) * radiance * std::abs(Dot(si.n, ls.wo)) * std::abs(Dot(light.normal, ls.wo)) * area / (ls.d * ls.d);
                                }
                            }
                        }
                   // }
                    result += color / samples;
                }
                else if (si2.didIntersect)
                {
                        result += si2.emissiveColor;
                }
            }

            this->outputImage.writePixelColor(result, x, y);
        }
    }
    auto finishTime = std::chrono::high_resolution_clock::now();

    return std::chrono::duration_cast<std::chrono::microseconds>(finishTime - startTime).count();
}

int main(int argc, char **argv)
{
    if (argc != 5)
    {
        std::cerr << "Usage: ./render <scene_config> <out_path> <num_samples> <sampling_strategy>";
        return 1;
    }
    Scene scene(argv[1]);

    Integrator rayTracer(scene);
    int spp = atoi(argv[3]);
    auto renderTime = 0;

    if (atoi(argv[4]) == 0)
        renderTime = rayTracer.render2(spp); // uniform

    if (atoi(argv[4]) == 1)
        renderTime = rayTracer.render3(spp); // cosine

    if (atoi(argv[4]) == 2)
        renderTime = rayTracer.render4(spp); // light sampling

    if (atoi(argv[4]) == 3)
        renderTime = rayTracer.render1(spp); // section 3

    if (atoi(argv[4]) == 4)
        renderTime = rayTracer.render(spp); // section 2

    std::cout << "Render Time: " << std::to_string(renderTime / 1000.f) << " ms" << std::endl;
    rayTracer.outputImage.save(argv[2]);

    return 0;
}
