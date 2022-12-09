#ifndef CYLINDER_H
#define CYLINDER_H

#include "utils/sceneparser.h"
#include <vector>
#include <glm/glm.hpp>

// Class for a unit Cylinder
class Cylinder
{
public:
    Cylinder();
    std::vector<float> updateParams(int param1, int param2);
    void storeRender(RenderShapeData *ptr);
    RenderShapeData *getRender();
private:
    void setVertexData();
    void makeTile(glm::vec3 topLeft,
                        glm::vec3 topRight,
                        glm::vec3 bottomLeft,
                        glm::vec3 bottomRight);
    void makeTileN(glm::vec3 topLeft,
                        glm::vec3 topRight,
                        glm::vec3 bottomLeft,
                        glm::vec3 bottomRight, int upOrDown);

    std::vector<float> m_vertexData;
    int m_param1;
    int m_param2;
    float m_radius = 0.5;
    RenderShapeData *m_ptr;
};

#endif // CYLINDER_H
