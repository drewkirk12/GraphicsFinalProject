#include "cylinder.h"
#include "shapefunctions.h"

Cylinder::Cylinder(){}

/**
 * @brief Cylinder::updateParams - calls setVertexData with new parameters
 * @param param1 - latitudinal parameter. Affects shape tesselation.
 * @param param2 - longudinal parameter. Affects shape tesselation.
 * @return - the vertex data of a tessellated unit cylinder
 */
std::vector<float> Cylinder::updateParams(int param1, int param2) {
    m_param1 = param1;
    m_param2 = fmax(3, param2);
    m_vertexData = std::vector<float>();
    setVertexData();
    return m_vertexData;
}

/**
 * @brief Cylinder::storeRender -
 * used to store pointer to the "first"
 * of a kind of shape when looping through shapes in realtime.cpp
 */
void Cylinder::storeRender(RenderShapeData *ptr) {
    m_ptr = ptr;
}

/**
 * @brief Cylinder::getRender - getter for the above pointer
 */
RenderShapeData *Cylinder::getRender() {
    return m_ptr;
}

/**
 * @brief Cylinder::setVertexData
 * - Most important part of preparing data to construct the
 * unit cylinder. - sets all vertices with respect to tessellation
 * and with respect to parameters.
 */
void Cylinder::setVertexData() {
    // making sides
    float currY = 0.5f;
    float nextY = 0.5f - (1.0f / m_param1);
    float currTheta = 2.0f * M_PI;
    float nextTheta = -(2.0f * M_PI) / m_param2;

    for (int j = 0; j < m_param1; j++) {
        for (int i = 0; i < m_param2; i++) {
            float xL = 0.5f * glm::cos(currTheta);
            float xR = 0.5f * glm::cos(nextTheta);
            float zL = 0.5f * glm::sin(currTheta);
            float zR = 0.5f * glm::sin(nextTheta);

            glm::vec3 TL{xL, currY, zL};
            glm::vec3 TR{xR, currY, zR};
            glm::vec3 BL{xL, nextY, zL};
            glm::vec3 BR{xR, nextY, zR};

            makeTile(TL, TR, BL, BR);
            currTheta += -((2.0f * M_PI) / m_param2);
            nextTheta += -((2.0f * M_PI) / m_param2);
        }
        currY -= (1.0f / m_param1);
        nextY -= (1.0f / m_param1);
    }

    // top cap
    // resetting angles & defining vertical normals for caps
    currTheta = 2.0f * M_PI;
    nextTheta = -(2.0f * M_PI) / m_param2;
    glm::vec3 upperNorm{0.0f, 1.0f, 0.0f};
    glm::vec3 lowerNorm{0.0f, -1.0f, 0.0f};
    // step toward the edge of the caps
    float outStep = 0.5f / m_param1;
    float nextOut = (0.5f / m_param1) + (0.5f / m_param1);

    // triangle from center of cap
    glm::vec3 center{0.0f, 0.5f, 0.0f};
    glm::vec3 cL{(outStep * glm::cos(currTheta)),
                0.5f, (outStep * glm::sin(currTheta))};
    glm::vec3 cR{(outStep * glm::cos(nextTheta)),
                0.5f, (outStep * glm::sin(nextTheta))};

    for (int i = 0; i < m_param2; i++) {
        cL = {(outStep * glm::cos(currTheta)),
              0.5f, (outStep * glm::sin(currTheta))};
        cR = {(outStep * glm::cos(nextTheta)),
              0.5f, (outStep * glm::sin(nextTheta))};
        shapeFunc::insertTriangle(m_vertexData,
                                  center, upperNorm,
                                  cL, upperNorm, cR,
                                  upperNorm);

        currTheta += -((2.0f * M_PI) / m_param2);
        nextTheta += -((2.0f * M_PI) / m_param2);
    }
    // resetting angles
    currTheta = 2.0f * M_PI;
    nextTheta = -(2.0f * M_PI) / m_param2;
    // making triangles for top cap
    for (int j = 0; j < (m_param1 - 1); j++) {
        for (int i = 0; i < m_param2; i++) {
            float xL = outStep * glm::cos(currTheta);
            float xR = outStep * glm::cos(nextTheta);
            float zL = outStep * glm::sin(currTheta);
            float zR = outStep * glm::sin(nextTheta);

            float xLN = nextOut * glm::cos(currTheta);
            float xRN = nextOut * glm::cos(nextTheta);
            float zLN = nextOut * glm::sin(currTheta);
            float zRN = nextOut * glm::sin(nextTheta);

            glm::vec3 TL{xL, 0.5f, zL};
            glm::vec3 TR{xR, 0.5f, zR};
            glm::vec3 BL{xLN, 0.5f, zLN};
            glm::vec3 BR{xRN, 0.5f, zRN};

            makeTileN(TL, TR, BL, BR, 1);
            currTheta += -((2.0f * M_PI) / m_param2);
            nextTheta += -((2.0f * M_PI) / m_param2);
        }
        outStep += (0.5f / m_param1);
        nextOut += (0.5f / m_param1);
    }

    outStep = 0.5f / m_param1;
    nextOut = (0.5f / m_param1) + (0.5f / m_param1);
    currTheta = 2.0f * M_PI;
    nextTheta = -(2.0f * M_PI) / m_param2;

    //bottom cap
    center = {0.0f, -0.5f, 0.0f};
    for (int i = 0; i < m_param2; i++) {
        cR = {(outStep * glm::cos(currTheta)),
              -0.5f, (outStep * glm::sin(currTheta))};
        cL = {(outStep * glm::cos(nextTheta)),
              -0.5f, (outStep * glm::sin(nextTheta))};
        shapeFunc::insertTriangle(m_vertexData, center, lowerNorm,
                                  cL, lowerNorm, cR, lowerNorm);

        currTheta += -((2.0f * M_PI) / m_param2);
        nextTheta += -((2.0f * M_PI) / m_param2);
    }
    // resetting angles
    currTheta = 2.0f * M_PI;
    nextTheta = -(2.0f * M_PI) / m_param2;
    for (int j = 0; j < (m_param1 - 1); j++) {
        for (int i = 0; i < m_param2; i++) {
            float xR = outStep * glm::cos(currTheta);
            float xL = outStep * glm::cos(nextTheta);
            float zR = outStep * glm::sin(currTheta);
            float zL = outStep * glm::sin(nextTheta);

            float xRN = nextOut * glm::cos(currTheta);
            float xLN = nextOut * glm::cos(nextTheta);
            float zRN = nextOut * glm::sin(currTheta);
            float zLN = nextOut * glm::sin(nextTheta);

            glm::vec3 TL{xL, -0.5f, zL};
            glm::vec3 TR{xR, -0.5f, zR};
            glm::vec3 BL{xLN, -0.5f, zLN};
            glm::vec3 BR{xRN, -0.5f, zRN};

            makeTileN(TL, TR, BL, BR, 0);
            currTheta += -((2.0f * M_PI) / m_param2);
            nextTheta += -((2.0f * M_PI) / m_param2);
        }
        outStep += (0.5f / m_param1);
        nextOut += (0.5f / m_param1);
    }

}

/**
 * @brief Cylinder::makeTileN - make tile function for top/bottom caps
 * @param tL, tR, bL, bR - TopLeft, TopRight, bottomLeft, bottomRight
 * @param upOrDown - decides positive or negative normal
 */
void Cylinder::makeTileN(glm::vec3 tL, glm::vec3 tR,
                         glm::vec3 bL, glm::vec3 bR, int upOrDown) {
    glm::vec3 norm;
    if (upOrDown) {
        norm = {0.0f, 1.0f, 0.0f};
    }
    else {
        norm = {0.0f, -1.0f, 0.0f};
    }
    // top and bottom triangles for tile
    shapeFunc::insertTriangle(m_vertexData, tL, norm, bR, norm, tR, norm);
    shapeFunc::insertTriangle(m_vertexData, tL, norm, bL, norm, bR, norm);
}

/**
 * @brief Cylinder::makeTile
 * @param tL, tR, bL, bR - TopLeft, TopRight, bottomLeft, bottomRight
 * Maketile function for the cylinder sides. Loop that calls this is above
 */
void Cylinder::makeTile(glm::vec3 tL, glm::vec3 tR,
                        glm::vec3 bL, glm::vec3 bR) {

    glm::vec3 n11{tL[0], 0.0f, tL[2]};
    glm::vec3 n21{bR[0], 0.0f, bR[2]};
    glm::vec3 n31{tR[0], 0.0f, tR[2]};
    glm::vec3 n22{bL[0], 0.0f, bL[2]};
    n11 = glm::normalize(n11);
    n21 = glm::normalize(n21);
    n31 = glm::normalize(n31);
    n22 = glm::normalize(n22);

    shapeFunc::insertTriangle(m_vertexData, tL, n11, bR, n21, tR, n31);
    shapeFunc::insertTriangle(m_vertexData, tL, n11, bL, n22, bR, n21);
}

