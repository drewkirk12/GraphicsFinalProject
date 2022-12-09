#ifndef CUBE_H
#define CUBE_H

#include "utils/sceneparser.h"
#include <vector>
#include <glm/glm.hpp>

/*
 * Class for the unit Cube
*/
class Cube
{
public:
    Cube();
    std::vector<float> updateParams(int param1);
    void storeRender(RenderShapeData *ptr);
    RenderShapeData *getRender();

private:
    void setVertexData();
    void makeTile(glm::vec3 topLeft,
                  glm::vec3 topRight,
                  glm::vec3 bottomLeft,
                  glm::vec3 bottomRight);
    void makeFace(glm::vec3 topLeft,
                  glm::vec3 topRight,
                  glm::vec3 bottomLeft,
                  glm::vec3 bottomRight);

    std::vector<float> m_vertexData;
    int m_param1;
    RenderShapeData *m_ptr;

};

#endif // CUBE_H
