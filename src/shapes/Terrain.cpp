#include "Terrain.h"

std::vector<float> Terrain::updateParams(int param1) {
    m_vertexData = std::vector<float>();
    m_param1 = param1;
    m_lookupSize = 1024;
    m_randVecLookup.reserve(m_lookupSize);

    // Initialize random number generator
    std::srand(1230);

    // Populate random vector lookup table
    for (int i = 0; i < m_lookupSize; i++)
    {
      m_randVecLookup.push_back(glm::vec2(std::rand() * 2.0 / RAND_MAX - 1.0,
                                          std::rand() * 2.0 / RAND_MAX - 1.0));
    }

    makeFace();
    return m_vertexData;
}

// ====================================== PERLIN HELPERS ====================================== //

// Helper for computePerlin() and, possibly, getColor()
float Terrain::interpolate(float A, float B, float alpha) {
    float ease = 3 * pow(alpha, 2) - 2 * pow(alpha, 3);
    return A + ease * (B - A);
}

// Samples the (infinite) random vector grid at (row, col)
glm::vec2 Terrain::sampleRandomVector(int row, int col) {
    std::hash<int> intHash;
    int index = intHash(row * 41 + col * 43) % m_lookupSize;
    return m_randVecLookup.at(index);
}

// Computes the intensity of Perlin noise at some point
float Terrain::computePerlin(float x, float y) {
    // Get grid indices (as ints)
    int X = (int)x;
    int Y = (int)y;
    glm::vec2 i1 = {X, Y};
    glm::vec2 i2 = {X + 1, Y};
    glm::vec2 i3 = {X, Y + 1};
    glm::vec2 i4 = {X + 1, Y + 1};

    // Compute offset vectors
    glm::vec2 input = {x,y};
    glm::vec2 off1 = input - i1;
    glm::vec2 off2 = input - i2;
    glm::vec2 off3 = input - i3;
    glm::vec2 off4 = input - i4;

    // Compute the dot product between offset and grid vectors
    float dot1 = glm::dot(off1, sampleRandomVector(i1[0], i1[1]));
    float dot2 = glm::dot(off2, sampleRandomVector(i2[0], i2[1]));
    float dot3 = glm::dot(off3, sampleRandomVector(i3[0], i3[1]));
    float dot4 = glm::dot(off4, sampleRandomVector(i4[0], i4[1]));

    // Use your interpolation function to produce the final value
    float inter1 = interpolate(dot1, dot2, x - i1[0]);
    float inter2 = interpolate(dot3, dot4, x - i1[0]);

    // Return 0 as a placeholder
    return interpolate(inter1, inter2, y - i1[1]);
}

// Takes a normalized (x, y) position, in range [0,1)
// Returns a height value, z, by sampling a noise function
float Terrain::getHeight(float x, float y) {

    float z = 0;

    // Combine multiple different octaves of noise to produce fractal perlin noise
    z += computePerlin(x * 8, y * 8) / 8;
    z += computePerlin(x * 16, y * 16) / 16;
    z += computePerlin(x * 32, y * 32) / 32;
    z += computePerlin(x * 64, y * 64) / 64;

    return z;
}

// ====================================== BASE PLANE ====================================== //

void Terrain::makeTile(glm::vec3 topLeft,
                    glm::vec3 topRight,
                    glm::vec3 bottomLeft,
                    glm::vec3 bottomRight) {

    //normals are destination - source
    glm::vec3 TLnormal = glm::normalize(glm::cross(bottomLeft - topLeft, topRight - topLeft));
    glm::vec3 TRnormal = glm::normalize(glm::cross(topLeft - topRight, bottomRight - topRight));
    glm::vec3 BLnormal = glm::normalize(glm::cross(bottomRight - bottomLeft, topLeft - bottomLeft));
    glm::vec3 BRnormal = glm::normalize(glm::cross(topRight - bottomRight, bottomLeft - bottomRight));

    // triangle 1
    insertVec3(m_vertexData, topLeft);
    insertVec3(m_vertexData, TLnormal);
    insertVec3(m_vertexData, bottomLeft);
    insertVec3(m_vertexData, BLnormal);
    insertVec3(m_vertexData, bottomRight);
    insertVec3(m_vertexData, BRnormal);

    // triangle 2
    insertVec3(m_vertexData, topLeft);
    insertVec3(m_vertexData, TLnormal);
    insertVec3(m_vertexData, bottomRight);
    insertVec3(m_vertexData, BRnormal);
    insertVec3(m_vertexData, topRight);
    insertVec3(m_vertexData, TRnormal);
}

void Terrain::makeFace() {

    glm::vec3 topLeft, topRight, bottomLeft, bottomRight;

    float m_resolution = 5.0;
    float m_terrainSize = 50.0;
    float m_halfRes = m_terrainSize / 2.0;
    float m_heightMultiplier = m_terrainSize; // terrain size gives best default results, but this can be modified as desired

    float xVal, xValLong;
    float yVal, yValLong;
    float zVal;


    float sideLength = m_terrainSize / (m_param1 * m_resolution);
    for (int x = 0; x < (m_param1 * m_resolution); x ++) {
        xVal = -m_halfRes + (x * sideLength);
        xValLong = xVal + sideLength;

        for (int y = 0; y < (m_param1 * m_resolution); y ++) {
            yVal = -m_halfRes + (y * sideLength);
            yValLong = yVal + sideLength;

            zVal = m_heightMultiplier * getHeight((xVal + m_halfRes) / m_terrainSize, (yValLong + m_halfRes) / m_terrainSize);
            topLeft = {xVal, zVal, yValLong};

            zVal = m_heightMultiplier * getHeight((xValLong + m_halfRes) / m_terrainSize, (yValLong + m_halfRes) / m_terrainSize);
            topRight = {xValLong, zVal, yValLong};

            zVal = m_heightMultiplier * getHeight((xVal + m_halfRes) / m_terrainSize, (yVal + m_halfRes) / m_terrainSize);
            bottomLeft = {xVal, zVal, yVal};

            zVal = m_heightMultiplier * getHeight((xValLong + m_halfRes) / m_terrainSize, (yVal + m_halfRes) / m_terrainSize);
            bottomRight = {xValLong, zVal, yVal};


            makeTile(topRight, topLeft, bottomRight, bottomLeft);
        }
    }

}

// Inserts a glm::vec3 into a vector of floats.
void Terrain::insertVec3(std::vector<float> &data, glm::vec3 v) {
    data.push_back(v.x);
    data.push_back(v.y);
    data.push_back(v.z);
}
