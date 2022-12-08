#ifndef RAY_H
#define RAY_H
#include "utils/scenedata.h"
#include <glm/glm.hpp>

// class representing the ray, more detailed comments in ray.cpp
class Ray
{
public:
    // member variables and functions
    Ray(glm::vec4 eye, glm::vec4 d);
    std::tuple<float, float> quadHelper(float a, float b, float c);
    bool discrimCheck(float a, float b, float c);
    std::tuple<float, glm::vec3, glm::vec3, int> intersectionCheck(PrimitiveType type);
    std::tuple<float, glm::vec3, glm::vec3, int> cylinderIntersect(float t1, float t2);
    float planeIntersect(float A, float B, float C, float D);
    std::tuple<float, glm::vec3, glm::vec3, int> coneIntersect(float t1, float t2);
    std::tuple<float, glm::vec3, glm::vec3, int> cubeIntersect();
    float px; float py; float pz; // starting position (x, y, z) of ray
    float dx; float dy; float dz; // ray direction (dx, dy, dz)
};

#endif // RAY_H
