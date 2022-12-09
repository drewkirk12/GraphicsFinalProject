#include "cube.h"
#include "shapefunctions.h"

Cube::Cube(){}

/**
 * @brief Cube::updateParams - updates tessellation/unit cube
 * @param param1 - latitudinal paramater. Affects tessellation of objects
 * @return - vertex data for the unit cube once done
 */
std::vector<float> Cube::updateParams(int param1) {
    m_vertexData = std::vector<float>();
    m_param1 = param1;
    setVertexData();
    return m_vertexData;
}
/**
 * @brief Cube::storeRender - used to store pointer to the "first"
 * of a kind of shape when looping through shapes in realtime.cpp
 * @param ptr
 */
void Cube::storeRender(RenderShapeData *ptr) {
    m_ptr = ptr;
}

/**
 * @brief Cube::getRender - getter for pointer above
 */
RenderShapeData *Cube::getRender() {
    return m_ptr;
}


/**
 * @brief Cube::makeTile
 * @param TopLeft, TopRight, bottomLeft, bottomRight
 * Maketile function that adds to the vertex data.
 */
void Cube::makeTile(glm::vec3 tL, glm::vec3 tR, glm::vec3 bL, glm::vec3 bR) {
    // first triangle (top)
    glm::vec3 n11 = glm::normalize(glm::cross((bR - tL), (tR - tL)));
    glm::vec3 n21 = glm::normalize(glm::cross((tR - bR), (tL - bR)));
    glm::vec3 n31 = glm::normalize(glm::cross((tL - tR), (bR - tR)));
    shapeFunc::insertTriangle(m_vertexData, tL, n11, bR, n21, tR, n31);
    // second triangle (bottom)
    glm::vec3 n12 = glm::normalize(glm::cross((bL - tL), (bR - tL)));
    glm::vec3 n22 = glm::normalize(glm::cross((bR - bL), (tL - bL)));
    glm::vec3 n32 = glm::normalize(glm::cross((tL - bR), (bL - bR)));
    shapeFunc::insertTriangle(m_vertexData, tL, n12, bL, n22, bR, n32);
}

/**
 * @brief Cube::makeFace - makes faces of cubes
 * @param TopLeft, TopRight, bottomLeft, bottomRight
 */
void Cube::makeFace(glm::vec3 topLeft,
                    glm::vec3 topRight,
                    glm::vec3 bottomLeft,
                    glm::vec3 bottomRight) {
    float sideLen = glm::distance(topRight, topLeft);
    sideLen = (float) (sideLen / (float) m_param1);

    // checker
    glm::vec3 check = topLeft + bottomRight;
    if (check[2] == 1.0) { // top
        glm::vec3 newTL = topLeft;
        for (int i = 0; i < m_param1; i++) {
            for (int j = 0; j < m_param1; j++) {
                //starting TL
                newTL = {topLeft[0] + sideLen * (i), topLeft[1]
                         - sideLen * j, topLeft[2]};
                glm::vec3 newTR{(newTL[0] + sideLen),
                               (newTL[1]), newTL[2]};
                glm::vec3 newBL{(newTL[0]),
                               (newTL[1] - sideLen), newTL[2]};
                glm::vec3 newBR{(newTL[0] + sideLen),
                               (newTL[1] - sideLen), newTL[2]};
                makeTile(newTL, newTR, newBL, newBR);
            }
        }
    }
    if (check[1] == 1.0) { // upper
        glm::vec3 newTL = topLeft;
        for (int i = 0; i < m_param1; i++) {
            for (int j = 0; j < m_param1; j++) {
                //starting TL
                newTL = {topLeft[0] + sideLen * (i), topLeft[1],
                         topLeft[2] + sideLen * j};
                glm::vec3 newTR{(newTL[0] + sideLen),
                               (newTL[1]), newTL[2]};
                glm::vec3 newBL{(newTL[0]),
                               (newTL[1]), newTL[2] + sideLen};
                glm::vec3 newBR{(newTL[0] + sideLen),
                               (newTL[1]), newTL[2] + sideLen};
                makeTile(newTL, newTR, newBL, newBR);
            }
        }
    }
    if (check[1] == -1.0) { //lower
        glm::vec3 newTL = topLeft;
        for (int i = 0; i < m_param1; i++) {
            for (int j = 0; j < m_param1; j++) {
                //starting TL
                newTL = {topLeft[0] + sideLen * (i), topLeft[1],
                         topLeft[2] - sideLen * j};
                glm::vec3 newTR{(newTL[0] + sideLen),
                               (newTL[1]), newTL[2]};
                glm::vec3 newBL{(newTL[0]),
                               (newTL[1]), newTL[2] - sideLen};
                glm::vec3 newBR{(newTL[0] + sideLen),
                               (newTL[1]), newTL[2] - sideLen};
                makeTile(newTL, newTR, newBL, newBR);
            }
        }
    }
    if (check[0] == -1.0) { //left
        glm::vec3 newTL = topLeft;
        for (int i = 0; i < m_param1; i++) {
            for (int j = 0; j < m_param1; j++) {
                //starting TL
                newTL = {topLeft[0], topLeft[1] - sideLen * j,
                         topLeft[2] - sideLen * (i)};
                glm::vec3 newTR{(newTL[0]),
                               (newTL[1] - sideLen), newTL[2]};
                glm::vec3 newBL{(newTL[0]),
                               (newTL[1]), newTL[2] - sideLen};
                glm::vec3 newBR{(newTL[0]),
                               (newTL[1] - sideLen), newTL[2] - sideLen};
                makeTile(newTL, newTR, newBL, newBR);
            }
        }
    }
    if (check[0] == 1.0) { //right
        glm::vec3 newTL = topLeft;
        for (int i = 0; i < m_param1; i++) {
            for (int j = 0; j < m_param1; j++) {
                //starting TL
                newTL = {topLeft[0], topLeft[1] + sideLen * j,
                         topLeft[2] - sideLen * (i)};
                glm::vec3 newTR{(newTL[0]),
                               (newTL[1] + sideLen), newTL[2]};
                glm::vec3 newBL{(newTL[0]),
                               (newTL[1]), newTL[2] - sideLen};
                glm::vec3 newBR{(newTL[0]),
                               (newTL[1] + sideLen), newTL[2] - sideLen};
                makeTile(newTL, newTR, newBL, newBR);
            }
        }
    }

    if (check[2] == -1.0) { //opposite
        glm::vec3 newTL = topLeft;
        for (int i = 0; i < m_param1; i++) {
            for (int j = 0; j < m_param1; j++) {
                //starting TL
                newTL = {topLeft[0] + sideLen * (i),
                         topLeft[1] + sideLen * j, topLeft[2]};
                glm::vec3 newTR{(newTL[0] + sideLen),
                               (newTL[1]), newTL[2]};
                glm::vec3 newBL{(newTL[0]),
                               (newTL[1] + sideLen), newTL[2]};
                glm::vec3 newBR{(newTL[0] + sideLen),
                               (newTL[1] + sideLen), newTL[2]};
                makeTile(newTL, newTR, newBL, newBR);
            }
        }
    }

}

/**
 * @brief Cube::setVertexData
 * - while visually unpleasing, this method calls the (6) key
 * calls to makeFace() to make the unit cube
 */
void Cube::setVertexData() {

     makeFace(glm::vec3(-0.5f,  0.5f, 0.5f),
              glm::vec3( 0.5f,  0.5f, 0.5f),
              glm::vec3(-0.5f, -0.5f, 0.5f),
              glm::vec3( 0.5f, -0.5f, 0.5f));
     makeFace(glm::vec3(-0.5f,  0.5f,-0.5f),
              glm::vec3( 0.5f,  0.5f,-0.5f),
              glm::vec3(-0.5f,  0.5f, 0.5f),
              glm::vec3( 0.5f,  0.5f, 0.5f));
     makeFace(glm::vec3(-0.5f, -0.5f, 0.5f),
              glm::vec3( 0.5f, -0.5f, 0.5f),
              glm::vec3(-0.5f, -0.5f, -0.5f),
              glm::vec3( 0.5f, -0.5f, -0.5f));
     makeFace(glm::vec3(-0.5f,  0.5f, 0.5f),
              glm::vec3(-0.5f, -0.5f, 0.5f),
              glm::vec3(-0.5f,  0.5f, -0.5f),
              glm::vec3(-0.5f, -0.5f, -0.5f));
     makeFace(glm::vec3( 0.5f, -0.5f, 0.5f),
              glm::vec3( 0.5f,  0.5f, 0.5f),
              glm::vec3( 0.5f, -0.5f, -0.5f),
              glm::vec3( 0.5f,  0.5f, -0.5f));
     makeFace(glm::vec3(-0.5f, -0.5f, -0.5f),
              glm::vec3( 0.5f, -0.5f, -0.5f),
              glm::vec3(-0.5f, 0.5f, -0.5f),
              glm::vec3( 0.5f, 0.5f, -0.5f));
}

