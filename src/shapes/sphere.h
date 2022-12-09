#ifndef SPHERE_H
#define SPHERE_H
#include "utils/sceneparser.h"
#include <vector>
#include <glm/glm.hpp>

// Class for the unit sphere
class Sphere
{
public:
    Sphere();
    std::vector<float> updateParams(int param1, int param2);
    void storeRender(RenderShapeData *ptr);
    RenderShapeData *getRender();

private:
    void setVertexData();
    void makeTile(glm::vec3 tL, glm::vec3 tR, glm::vec3 bL, glm::vec3 bR);
    void makeWedge(float currTheta, float nextTheta);
    void makeSphere();

    std::vector<float> m_vertexData;
    int m_param1;
    int m_param2;
    RenderShapeData *m_ptr;
};

#endif // SPHERE_H
