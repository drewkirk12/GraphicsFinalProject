#include "cone.h"
#include "shapefunctions.h"


Cone::Cone(){}

/**
 * @brief Cone::updateParams - calls setVertexData(), returns its results
 * @param param1 - latitudinal parameter. Affects shape tesselation.
 * @param param2 - longudinal parameter. Affects shape tesselation.
 * @return - vertex data for the unit cone
 */
std::vector<float> Cone::updateParams(int param1, int param2) {
    m_param1 = param1;
    m_param2 = fmax(3, param2);
    m_vertexData = std::vector<float>();

    setVertexData();
    return m_vertexData;
}

/**
 * @brief Cone::storeRender - used to store pointer to the "first"
 * of a kind of shape when looping through shapes in realtime.cpp
 * @param ptr
 */
void Cone::storeRender(RenderShapeData *ptr) {
    m_ptr = ptr;
}
/**
 * @brief Cone::getRender - getter for the above pointer
 */
RenderShapeData *Cone::getRender() {
    return m_ptr;
}

// helper function to find radius of cone for a given height
float getR(float y) {
    return (-0.5f * (y - 0.5f));
}

/**
 * @brief Cone::setVertexData - prepares ALL vertex/normals
 * - Big method handles all of cone vertices and normals calculations
 */
void Cone::setVertexData() {
    // making sides
    float currY = 0.5f;
    float nextY = 0.5f - (1.0f / m_param1);
    float currTheta = 2.0f * M_PI;
    float nextTheta = -(2.0f * M_PI) / m_param2;

    // making cone tip
    glm::vec3 tip{0.0f, 0.5f, 0.0f};
    for (int i = 0; i < m_param2; i++) {
        float xL = getR(nextY) * glm::cos(currTheta);
        float xR = getR(nextY) * glm::cos(nextTheta);
        float zL = getR(nextY) * glm::sin(currTheta);
        float zR = getR(nextY) * glm::sin(nextTheta);

        glm::vec3 tipL{xL, nextY, zL};
        glm::vec3 tipR{xR, nextY, zR};

        glm::vec3 npL{(2.0f * tipL[0]), ((-0.5f * tipL[1]) + 0.25f),
                                                  (2.0f * tipL[2])};
        glm::vec3 npR{(2.0f * tipR[0]), ((-0.5f * tipR[1]) + 0.25f),
                                                  (2.0f * tipR[2])};
        npL = glm::normalize(npL);
        npR = glm::normalize(npR);

        float tipNormX = getR(nextY) * glm::cos(currTheta
                          - (0.5f * ((2.0f * M_PI) / m_param2)));
        float tipNormZ = getR(nextY) * glm::sin(currTheta
                          - (0.5f * ((2.0f * M_PI) / m_param2)));
        glm::vec3 tipNorm{(2.0f * tipNormX), ((-0.5f * nextY)
                                + 0.25f), (2.0f * tipNormZ)};
        tipNorm = normalize(tipNorm);

        shapeFunc::insertTriangle(m_vertexData, tip, tipNorm, tipL,
                                                   npL, tipR, npR);

        currTheta += -((2.0f * M_PI) / m_param2);
        nextTheta += -((2.0f * M_PI) / m_param2);
    }
    // resetting angles & y-value
    currTheta = 2.0f * M_PI;
    nextTheta = -(2.0f * M_PI) / m_param2;
    currY -= (1.0f / m_param1);
    nextY -= (1.0f / m_param1);

    // making pyramidal sides
    for (int j = 0; j < (m_param1 - 1); j++) {
        for (int i = 0; i < m_param2; i++) {
            float xL = getR(currY) * glm::cos(currTheta);
            float xR = getR(currY) * glm::cos(nextTheta);
            float zL = getR(currY) * glm::sin(currTheta);
            float zR = getR(currY) * glm::sin(nextTheta);

            float xLN = getR(nextY) * glm::cos(currTheta);
            float xRN = getR(nextY) * glm::cos(nextTheta);
            float zLN = getR(nextY) * glm::sin(currTheta);
            float zRN = getR(nextY) * glm::sin(nextTheta);

            glm::vec3 TL{xL, currY, zL};
            glm::vec3 TR{xR, currY, zR};
            glm::vec3 BL{xLN, nextY, zLN};
            glm::vec3 BR{xRN, nextY, zRN};

            makeTile(TL, TR, BL, BR);
            currTheta += -((2.0f * M_PI) / m_param2);
            nextTheta += -((2.0f * M_PI) / m_param2);
        }
        currY -= (1.0f / m_param1);
        nextY -= (1.0f / m_param1);
    }

    // bottom cap
    glm::vec3 lowerNorm{0.0f, -1.0f, 0.0f};
    // establishing step toward edge of cone & resetting theta
    float outStep = 0.5f / m_param1;
    float nextOut = (0.5f / m_param1) + (0.5f / m_param1);
    currTheta = 2.0f * M_PI;
    nextTheta = -(2.0f * M_PI) / m_param2;

    //bottom cap
    glm::vec3 center = {0.0f, -0.5f, 0.0f};
    for (int i = 0; i < m_param2; i++) {
        glm::vec3 cR = {(outStep * glm::cos(currTheta)),
                        -0.5f, (outStep * glm::sin(currTheta))};
        glm::vec3 cL = {(outStep * glm::cos(nextTheta)),
                        -0.5f, (outStep * glm::sin(nextTheta))};
        shapeFunc::insertTriangle(m_vertexData, center,
                                  lowerNorm, cL, lowerNorm, cR, lowerNorm);

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

void Cone::makeTileN(glm::vec3 topLeft,
                    glm::vec3 topRight,
                    glm::vec3 bottomLeft,
                    glm::vec3 bottomRight, int upOrDown) {
    glm::vec3 norm;
    if (upOrDown) {
        norm = {0.0f, 1.0f, 0.0f};
    }
    else {
        norm = {0.0f, -1.0f, 0.0f};
    }

    // first triangle (top)
    glm::vec3 v11 = topLeft;
    glm::vec3 v21 = bottomRight;
    glm::vec3 v31 = topRight;

    shapeFunc::insertTriangle(m_vertexData, v11, norm, v21, norm, v31, norm);

    glm::vec3 v12 = topLeft;
    glm::vec3 v22 = bottomLeft;
    glm::vec3 v32 = bottomRight;

    // NORMAL DIFFERENTLY
    shapeFunc::insertTriangle(m_vertexData, v12, norm, v22, norm, v32, norm);
}

void Cone::makeTile(glm::vec3 topLeft,
                    glm::vec3 topRight,
                    glm::vec3 bottomLeft,
                    glm::vec3 bottomRight) {
    // first triangle (top)
    glm::vec3 v11 = topLeft;
    glm::vec3 v21 = bottomRight;
    glm::vec3 v31 = topRight;

    glm::vec3 n11{(2.0f * v11[0]), ((-0.5f * v11[1]) + 0.25f),
                                              (2.0f * v11[2])};
    glm::vec3 n21{(2.0f * v21[0]), ((-0.5f * v21[1]) + 0.25f),
                                              (2.0f * v21[2])};
    glm::vec3 n31{(2.0f * v31[0]), ((-0.5f * v31[1]) + 0.25f),
                                             (2.0f * v31[2])};
    n11 = glm::normalize(n11);
    n21 = glm::normalize(n21);
    n31 = glm::normalize(n31);
    shapeFunc::insertTriangle(m_vertexData, v11, n11, v21, n21, v31, n31);

    glm::vec3 v12 = topLeft;
    glm::vec3 v22 = bottomLeft;
    glm::vec3 v32 = bottomRight;

    glm::vec3 n12{(2.0f * v12[0]), ((-0.5f * v12[1]) + 0.25f),
                                             (2.0f * v12[2])};
    glm::vec3 n22{(2.0f * v22[0]), ((-0.5f * v22[1]) + 0.25f),
                                             (2.0f * v22[2])};
    glm::vec3 n32{(2.0f * v32[0]), ((-0.5f * v32[1]) + 0.25f),
                                             (2.0f * v32[2])};
    n12 = glm::normalize(n12);
    n22 = glm::normalize(n22);
    n32 = glm::normalize(n32);

    // NORMAL DIFFERENTLY
    shapeFunc::insertTriangle(m_vertexData, v12, n12, v22, n22, v32, n32);
}


