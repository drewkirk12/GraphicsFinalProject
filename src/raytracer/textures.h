#include "qimage.h"
#ifndef TEXTURES_H
#define TEXTURES_H


#include "raytracer/raytracescene.h"
class Textures
{
public:
    Textures();
    void loadImages(const RayTraceScene &scene, std::vector<std::vector<RGBA>> &allImages, std::vector<int> &allWidths, std::vector<int> &allHeights);
    void loadImageFromFile(const QString &file, int &width, int &height, std::vector<RGBA> &image);
    int uvCoordtoImage(RenderShapeData renderable, glm::vec3 interPoint, int intersectionMarker, int tWidth, int tHeight);
    int uvToRowCol(float u, float v, int textureWidth, int textureHeight);
    int uvMulti(float u, float v, int textureWidth, int textureHeight, int repeatU, int repeatV);
    std::tuple<float, float> planeUV(float px, float pz);
    float perimeterU(float px, float pz);
    float sphereV(float py);
};

#endif // TEXTURES_H
