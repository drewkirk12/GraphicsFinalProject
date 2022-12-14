#include "heightgrad.h"

#include <vector>

#include <glm/glm.hpp>

#include "params.h"

namespace cloud {

void computeDensities(unsigned int height, float resolution,
                      std::vector<GLfloat> &densities);

void computeGradients(unsigned int height,
                      float resolution,
                      const std::vector<GLfloat> &densities,
                      std::vector<GLfloat> &gradients);

void generateHeightGradient(
        int height, float resolution,
        GLuint densityTex, GLuint gradTex) {
    int numSteps = height * resolution;
    height = numSteps / resolution;

    assert(numSteps > 0);

    std::vector<GLfloat> densities;
    std::vector<GLfloat> gradients;

    computeDensities(height, resolution, densities);
    computeGradients(height, resolution, densities, gradients);
    // Compute derivative

    glBindTexture(GL_TEXTURE_1D, densityTex);
    glTexImage1D(GL_TEXTURE_1D,
                 0, // level
                 GL_RED, // internalformat
                 numSteps,
                 0, // border
                 GL_RED, // format
                 GL_FLOAT,
                 densities.data());
    glBindTexture(GL_TEXTURE_1D, 0);
    glBindTexture(GL_TEXTURE_1D, gradTex);
    glTexImage1D(GL_TEXTURE_1D,
                 0, // level
                 GL_RED, // internalformat
                 numSteps,
                 0, // border
                 GL_RED, // format
                 GL_FLOAT,
                 gradients.data());
    glBindTexture(GL_TEXTURE_1D, 0);

}

void computeDensities(unsigned int height, float resolution,
                      std::vector<GLfloat> &densities) {
    densities.assign(height * resolution, densityScale);

    for (unsigned int i = 0; i < densities.size(); i++) {
        GLfloat &density = densities[i];
        float h = i / resolution;
        // Only allow clouds at some heights
        density *= glm::smoothstep(cloudFloorStart, cloudFloorEnd, h)
                - glm::smoothstep(cloudCeilStart, cloudCeilEnd, h);
        // Decrease density at high altitudes
        // density *= glm::min(1.f, 10.f / (10 + h));
    }
}

void computeGradients(unsigned int height,
                      float resolution,
                      const std::vector<GLfloat> &densities,
                      std::vector<GLfloat> &gradients) {
    int numSteps = densities.size();
    gradients.assign(numSteps, 0);

    gradients[0] = (densities[1] - densities[1])
            * resolution
            * height;
    for (int i = 1; i < numSteps - 1; i++) {
        gradients[i] =
                (densities[i + 1] - densities[i - 1])
                * (2.0 * resolution)
                * height;
    }
    gradients[numSteps - 1] =
            (densities[numSteps - 1]
            - densities[numSteps - 2])
            * resolution
            * height;
}

}
