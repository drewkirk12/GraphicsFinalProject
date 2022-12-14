#include "noise.h"

#include <iostream>
#include <vector>

#include <glm/glm.hpp>
#include <glm/gtc/random.hpp>

#include "params.h"

namespace cloud {

inline int getIndex(glm::ivec3 coord, int dimSize) {
    glm::ivec3 wrapped = {
        coord.x % dimSize,
        coord.y % dimSize,
        coord.z % dimSize
    };
    return ((wrapped[0] * dimSize) + wrapped[1]) * dimSize + wrapped[2];
}

void addGradient(
        unsigned int noiseResolution,
        unsigned int sampleResolution,
        std::vector<glm::vec4> &texData,
        std::vector<glm::vec3> &gradData);

void generateNoise(
        unsigned int sampleResolution,
        GLuint noiseTex, GLuint gradTex) {
    std::vector<glm::vec4> texData(sampleResolution * sampleResolution * sampleResolution);
    // A gradient vector of the density
    std::vector<glm::vec3> gradData(sampleResolution * sampleResolution * sampleResolution);

    // Generate and sample noise of different resolutions
    addGradient(2, sampleResolution, texData, gradData);
    addGradient(4, sampleResolution, texData, gradData);
    addGradient(8, sampleResolution, texData, gradData);
    addGradient(16, sampleResolution, texData, gradData);

    // Pass noise texture

    glBindTexture(GL_TEXTURE_3D, noiseTex);
    glTexImage3D(GL_TEXTURE_3D,
                 0, // level
                 GL_RGBA, // internalformat
                 sampleResolution,
                 sampleResolution,
                 sampleResolution,
                 0, // border
                 GL_RGBA, // format
                 GL_FLOAT,
                 texData.data());
    glBindTexture(GL_TEXTURE_3D, 0);

    glBindTexture(GL_TEXTURE_3D, gradTex);
    glTexImage3D(GL_TEXTURE_3D,
                 0, // level
                 GL_RGB, // internalformat
                 sampleResolution,
                 sampleResolution,
                 sampleResolution,
                 0, // border
                 GL_RGB, // format
                 GL_FLOAT,
                 gradData.data());
    glBindTexture(GL_TEXTURE_3D, 0);
}

void initGradients(std::vector<glm::vec3> &gradients);
void sampleGradients(const std::vector<glm::vec3> &gradients,
                     unsigned int noiseResolution,
                     unsigned int sampleResolution,
                     std::vector<glm::vec4> &texData,
                     std::vector<glm::vec3> &gradData);

void addGradient(unsigned int noiseResolution,
                 unsigned int sampleResolution,
                 std::vector<glm::vec4> &texData,
                 std::vector<glm::vec3> &gradData) {

    // Generate textureSize * textureSize vec4s
    std::vector<glm::vec3> gradients(noiseResolution * noiseResolution * noiseResolution);
    initGradients(gradients);
    sampleGradients(gradients,
                    noiseResolution,
                    sampleResolution,
                    texData,
                    gradData);
}

void initGradients(std::vector<glm::vec3> &gradients) {
    for (size_t i = 0; i < gradients.size(); i++) {
        gradients[i] = glm::sphericalRand<GLfloat>(1);
    }
}

void sampleGradients(const std::vector<glm::vec3> &gradients,
                     unsigned int noiseResolution,
                     unsigned int sampleResolution,
                     std::vector<glm::vec4> &texData,
                     std::vector<glm::vec3> &gradData) {

    assert(gradients.size() ==
           noiseResolution * noiseResolution * noiseResolution);

    // At each texture coordinate, sample the gradients and compute
    //  (1) color
    //  (2) density
    //  (3) local gradient (points in direction of denser clouds)
    for (unsigned int i1 = 0; i1 < sampleResolution; i1++) {
    for (unsigned int i2 = 0; i2 < sampleResolution; i2++) {
    for (unsigned int i3 = 0; i3 < sampleResolution; i3++) {
        glm::ivec3 indexCoord(i1, i2, i3);
        int index = getIndex(indexCoord, sampleResolution);

        // Back-sampled coordinate, from 0.5 to sampleResolution - 0.5
        glm::vec3 coord = (glm::vec3(indexCoord) + glm::vec3(0.5))
                / ((float)sampleResolution);

        // Scale coordinate to gradient pixel space
        glm::vec3 gradientScaledCoord = coord * ((float)noiseResolution);
        glm::ivec3 corner = glm::floor(gradientScaledCoord);
        glm::vec3 offset = gradientScaledCoord - glm::vec3(corner);

        float alpha = 0;
        glm::vec3 localGrad(0);

        // Sample gradient from each corner
        for (int j1 = 0; j1 < 2; j1++) {
        for (int j2 = 0; j2 < 2; j2++) {
        for (int j3 = 0; j3 < 2; j3++) {
            glm::ivec3 selectedCorner(j1, j2, j3);
            glm::vec3 localOffset = offset - glm::vec3(selectedCorner);

            glm::vec3 gradient = gradients[
                    getIndex(corner + selectedCorner, noiseResolution)];
            glm::vec3 nearness = glm::vec3(1) - glm::abs(localOffset);
            float weight = nearness.x * nearness.y * nearness.z;

            float dot = glm::dot(localOffset, gradient);
            alpha += dot * weight;
            localGrad += gradient * weight;
        }
        }
        }

        glm::vec3 color = glm::vec3(0.8);
        texData[index] += glm::vec4(color, alpha) / (float) noiseResolution;
        gradData[index] += localGrad / (float) noiseResolution * (float) sampleResolution;
    }
    }
    }
}

}
