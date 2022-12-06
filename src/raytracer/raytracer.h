#pragma once

#include <glm/glm.hpp>
#include "utils/rgba.h"
#include "qimage.h"
#include "utils/scenedata.h"
#include "utils/sceneparser.h"
#include "textures.h"
#include <vector>

// A forward declaration for the RaytraceScene class

class RayTraceScene;

// A class representing a ray-tracer

class RayTracer
{
public:
    struct Config {
        bool enableShadow        = false;
        bool enableReflection    = false;
        bool enableRefraction    = false;
        bool enableTextureMap    = false;
        bool enableTextureFilter = false;
        bool enableParallelism   = false;
        bool enableSuperSample   = false;
        bool enableAcceleration  = false;
        bool enableDepthOfField  = false;
    };

public:
    RayTracer(Config config);

    Textures textures;

    // Renders the scene synchronously.
    // The ray-tracer will render the scene and fill imageData in-place.
    // @param imageData The pointer to the imageData to be filled.
    // @param scene The scene to be rendered.
    void render(RGBA *imageData, const RayTraceScene &scene);
    // my methods
    RGBA toRGBA(const glm::vec4 &illumination);

    float falloff(float angleX, float t_Inner, float t_Outer);
    float angleBTWVectors(glm::vec3 ang1, glm::vec3 ang2);
    int shadowCheck(glm::vec3 interPoint, glm::vec3 dir, glm::vec3 lightPos, const RayTraceScene &scene);

    void loadImages(const RayTraceScene &scene); //, std::vector<std::vector<RGBA>> &imageMaps);
    void loadImageFromFile(const QString &file, int &m_width, int &m_height, std::vector<RGBA> &image);
    int uvToRowCol(float u, float v, int textureWidth, int textureHeight);
    int uvMulti(float u, float v, int textureWidth, int textureHeight, int repeatU, int repeatV);
    float perimeterU(float px, float pz);
    float sphereV(float py);
    std::tuple<float, float> planeUV(float px, float pz);
    glm::vec4 traceRay(glm::vec3 start, glm::vec3 dir, const RayTraceScene &scene, int depth, float epsilon, float recurOffset);
    glm::vec4 recursiveRaytrace(glm::vec3 interPoint, glm::vec3 dir, SceneMaterial material, const RayTraceScene &scene, int depth);
    glm::vec4 phong(glm::vec3 position, glm::vec3  normal,
               glm::vec3 dirToCam, SceneMaterial material,
               const RayTraceScene &scene, int depth, RGBA texture);

    std::vector<std::vector<RGBA>> allImages;
    std::vector<int> allWidths;
    std::vector<int> allHeights;
private:
    const Config m_config;
};

