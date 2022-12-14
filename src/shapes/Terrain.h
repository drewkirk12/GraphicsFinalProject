#pragma once

#include <vector>
#include <glm/glm.hpp>

class Terrain
{
public:
    std::vector<float> updateParams(int param1);
//    std::vector<float> generateShape() { return m_vertexData; }


private:
    std::vector<float> m_vertexData;
    std::vector<glm::vec2> m_randVecLookup;

    glm::vec2 sampleRandomVector(int row, int col);

    int m_lookupSize;
    int m_param1;

    float computePerlin(float x, float y);
    float getHeight(float x, float y);
    float interpolate(float A, float B, float alpha);

    void insertVec3(std::vector<float> &data, glm::vec3 v);
    void makeTile(glm::vec3 topLeft,
                  glm::vec3 topRight,
                  glm::vec3 bottomLeft,
                  glm::vec3 bottomRight);
    void makeFace();

};
