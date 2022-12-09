#include "sphere.h"
#include <cmath>
#include "shapefunctions.h"

// ========== class representing a sphere

Sphere::Sphere() {}

/**
 * @brief Sphere::updateParams - calls makeSphere to create with new parameters
 * @param param1 - latitudinal parameter. Affects shape tesselation.
 * @param param2 - longudinal parameter. Affects shape tesselation.
 * @return - the vertex data of a tessellated unit sphere
 */
std::vector<float> Sphere::updateParams(int param1, int param2) {
    m_vertexData = std::vector<float>();
    m_param1 = fmax(2, param1);
    m_param2 = fmax(3, param2);

    makeSphere();
    return m_vertexData;
}

/**
 * @brief Sphere::storeRender - used to store pointer to the "first"
 * of a kind of shape when looping through shapes in realtime.cpp
 * @param ptr
 */
void Sphere::storeRender(RenderShapeData *ptr) {
    m_ptr = ptr;
}

/**
 * @brief Sphere::getRender - getter function of the pointer above
 */
RenderShapeData *Sphere::getRender() {
    return m_ptr;
}

/**
 * @brief Sphere::makeSphere
 * - Begins the process by calling makeWedge for the number of wedges needed
 * - Uses "theta" and "phi" from spherical coordinates
 */
void Sphere::makeSphere() {
    float thetaStep = glm::radians(360.f / m_param2);

    for (float i = 0.0; i <= (2.0 * M_PI); i += thetaStep) {
        float currTheta = i;
        float nextTheta = i + thetaStep;
        makeWedge(currTheta, nextTheta);
    }
}

/**
 * @brief Sphere::makeTile - makes a "square" tile from (2) triangles
 * @param tL, tR, bL, bR - TopLeft, TopRight, bottomLeft, bottomRight of tile
 */
void Sphere::makeTile(glm::vec3 tL, glm::vec3 tR, glm::vec3 bL, glm::vec3 bR) {

    glm::vec n11 = glm::normalize(tL);
    glm::vec n21 = glm::normalize(bR);
    glm::vec n31 = glm::normalize(tR);
    glm::vec n22 = glm::normalize(bL);
    // first triangle (top)
    shapeFunc::insertTriangle(m_vertexData, tL, n11, bR, n21, tR, n31);
    // second triangle (bottom)
    shapeFunc::insertTriangle(m_vertexData, tL, n11, bL, n22, bR, n21);
}

void Sphere::makeWedge(float currentTheta, float nextTheta) {
    float phiStep = glm::radians(180.f / m_param1);

    // top most triangle
    glm::vec3 sTop{0.0f, 0.5f, 0.0f};
    glm::vec3 sBL{0.5 * glm::sin(phiStep) * glm::sin(currentTheta),
                  0.5 * glm::cos(phiStep),
                  0.5 * glm::sin(phiStep) * glm::cos(currentTheta)};
    glm::vec3 sBR{0.5 * glm::sin(phiStep) * glm::sin(nextTheta),
                  0.5 * glm::cos(phiStep),
                  0.5 * glm::sin(phiStep) * glm::cos(nextTheta)};
    glm::vec3 nSTOP = glm::normalize(sTop);
    glm::vec3 nSBL = glm::normalize(sBL);
    glm::vec3 nSBR = glm::normalize(sBR);

    shapeFunc::insertTriangle(m_vertexData, sTop, nSTOP, sBL, nSBL, sBR, nSBR);

    //making interstitial triangles
    for (float i = phiStep; i < (M_PI - phiStep); i += phiStep) {
        float xTL = 0.5 * glm::sin(i) * glm::sin(currentTheta);
        float yTL = 0.5 * glm::cos(i);
        float zTL = 0.5 * glm::sin(i) * glm::cos(currentTheta);
        glm::vec3 topLeft{xTL, yTL, zTL};
        float xTR = 0.5 * glm::sin(i) * glm::sin(nextTheta);
        float yTR = 0.5 * glm::cos(i);
        float zTR = 0.5 * glm::sin(i) * glm::cos(nextTheta);
        glm::vec3 topRight{xTR, yTR, zTR};
        float xBL = 0.5 * glm::sin(i + phiStep) * glm::sin(currentTheta);
        float yBL = 0.5 * glm::cos(i + phiStep);
        float zBL = 0.5 * glm::sin(i + phiStep) * glm::cos(currentTheta);
        glm::vec3 bottomLeft{xBL, yBL, zBL};
        float xBR = 0.5 * glm::sin(i + phiStep) * glm::sin(nextTheta);
        float yBR = 0.5 * glm::cos(i + phiStep);
        float zBR = 0.5 * glm::sin(i + phiStep) * glm::cos(nextTheta);
        glm::vec3 bottomRight{xBR, yBR, zBR};
        makeTile(topLeft, topRight, bottomLeft, bottomRight);
    }

    // bottom triangle
    glm::vec3 bBott{0.0f, -0.5f, 0.0f};
    float xBTL = 0.5 * glm::sin(M_PI - phiStep) * glm::sin(currentTheta);
    float yBTL = 0.5 * glm::cos(M_PI - phiStep);
    float zBTL = 0.5 * glm::sin(M_PI - phiStep) * glm::cos(currentTheta);
    glm::vec3 bTL{xBTL, yBTL, zBTL};
    float xBTR = 0.5 * glm::sin(M_PI - phiStep) * glm::sin(nextTheta);
    float yBTR = 0.5 * glm::cos(M_PI - phiStep);
    float zBTR = 0.5 * glm::sin(M_PI - phiStep) * glm::cos(nextTheta);
    glm::vec3 bTR{xBTR, yBTR, zBTR};
    glm::vec3 nBott = glm::normalize(bBott);
    glm::vec3 nBTL = glm::normalize(bTL);
    glm::vec3 nBTR = glm::normalize(bTR);

    shapeFunc::insertTriangle(m_vertexData, bTL, nBTL, bBott, nBott, bTR, nBTR);
}
