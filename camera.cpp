#include "camera.h"

Camera::Camera(Vector3f from, Vector3f to, Vector3f up, float fieldOfView, Vector2i imageResolution)
    : from(from),
      to(to),
      up(up),
      fieldOfView(fieldOfView),
      imageResolution(imageResolution)
{
    this->aspect = imageResolution.x / float(imageResolution.y);

    // Determine viewport dimensions in 3D
    float fovRadians = fieldOfView * M_PI / 180.f;
    float h = std::tan(fovRadians / 2.f);
    float viewportHeight = 2.f * h * this->focusDistance;
    float viewportWidth = viewportHeight * this->aspect;

    // Calculate basis vectors of the camera for the given transform
    this->w = Normalize(this->from - this->to);
    this->u = Normalize(Cross(up, this->w));
    this->v = Normalize(Cross(this->w, this->u));

    // Pixel delta vectors
    Vector3f viewportU = viewportWidth * this->u;
    Vector3f viewportV = viewportHeight * (-this->v);

    this->pixelDeltaU = viewportU / float(imageResolution.x);
    this->pixelDeltaV = viewportV / float(imageResolution.y);

    // Upper left
    this->upperLeft = from - this->w * this->focusDistance - viewportU / 2.f - viewportV / 2.f;
}

// Ray Camera::generateRay(int x, int y, int samples)
// {

//     Vector3f directionSum = Vector3f(0.0f, 0.0f, 0.0f);

//     for (int s = 0; s < samples; ++s)
//     {
//         // Generate random offsets within the pixel
//         float epsilonx = next_float();
//         float epsilony = next_float();

//         // Calculate the position of the sample within the pixel
//         Vector3f pixelCenter = this->upperLeft + epsilonx * this->pixelDeltaU + epsilony * this->pixelDeltaV;
//         pixelCenter = pixelCenter + x * this->pixelDeltaU + y * this->pixelDeltaV;
//         // Vector3f pixelCenter = this->upperLeft- this->from + (x + epsilonx) * this->pixelDeltaU + (y + epsilony) * this->pixelDeltaV;
//         Vector3f direction = Normalize(pixelCenter - this->from);

//         directionSum += direction;
//     }

//     // Average the directions of all samples
//     Vector3f averagedDirection = directionSum / static_cast<float>(samples);

//     return Ray(this->from, averagedDirection);

// }

Ray Camera::generateRay(int x, int y, float epsilon1, float epsilon2)
{

    // Generate random offsets within the pixel

    // Calculate the position of the sample within the pixel
     Vector3f pixelCenter = this->upperLeft + epsilon1 * this->pixelDeltaU + epsilon2 * this->pixelDeltaV;
   // Vector3f pixelCenter = this->upperLeft + epsilon1 *(this->pixelDeltaU + this->pixelDeltaV);
    pixelCenter = pixelCenter + x * this->pixelDeltaU + y * this->pixelDeltaV;
   // Vector3f pixelCenter = this->upperLeft + (x + epsilon1) * this->pixelDeltaU + (y + epsilon2) * this->pixelDeltaV;
    Vector3f direction = Normalize(pixelCenter - this->from);

    return Ray(this->from, direction);





    // Vector3f directionSum = Vector3f(0.0f, 0.0f, 0.0f);

    // for (int s = 0; s < epsilon1; ++s)
    // {
    //     // Generate random offsets within the pixel
    //     float epsilonx = next_float();
    //     float epsilony = next_float();

    //     // Calculate the position of the sample within the pixel
    //     Vector3f pixelCenter = this->upperLeft + epsilonx * this->pixelDeltaU + epsilony * this->pixelDeltaV;
    //     pixelCenter = pixelCenter + x * this->pixelDeltaU + y * this->pixelDeltaV;
    //     // Vector3f pixelCenter = this->upperLeft- this->from + (x + epsilonx) * this->pixelDeltaU + (y + epsilony) * this->pixelDeltaV;
    //     Vector3f direction = Normalize(pixelCenter - this->from);

    //     directionSum += direction;
    // }

    // // Average the directions of all samples
    // Vector3f averagedDirection = directionSum / static_cast<float>(epsilon1);

    // return Ray(this->from, averagedDirection);
}

// Ray Camera::generateRay(int x, int y, int samples) {
//     // Generate random offsets within the pixel
//     float epsilonx = next_float();
//     float epsilony = next_float();

//     // Calculate the position of the sample within the pixel
//     Vector3f pixelPosition = this->upperLeft + ((x + 0.5f) * this->pixelDeltaU) + ((y + 0.5f) * this->pixelDeltaV);

//     // Calculate the direction from the camera position to the pixel position
//     Vector3f direction = Normalize(pixelPosition - this->from);

//     return Ray(this->from, direction);
// }
