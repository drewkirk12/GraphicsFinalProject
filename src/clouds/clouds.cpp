#include "clouds.h"

#include <iostream>

#include <glm/gtc/random.hpp>

#include <utils/shaderloader.h>

namespace cloud {

bool initialized = false;

GLuint cloudProgram;

GLuint sliceVBO;
GLuint sliceVAO;
int numQuads;

GLuint noiseTex;
GLuint gradTex;
int sampleResolution;

float sliceDistance = 0.2;

const Camera *camera;


// Utils

void addVector(std::vector<GLfloat> &data, float x, float y, float z) {
    data.push_back(x);
    data.push_back(y);
    data.push_back(z);
}


void defineSlicePlanes(float far);

void finalizeClouds() {
    glDeleteProgram(cloudProgram);
}

void initializeClouds() {
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    // https://stackoverflow.com/questions/17763021/opengl-blending-renders-black-on-black-as-gray
    glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA,
                        GL_ONE,       GL_ONE_MINUS_SRC_ALPHA);
    // Initialize planes
    cloudProgram = ShaderLoader::createShaderProgram(
                "./resources/shaders/cloud.vert",
                "./resources/shaders/cloud.frag");

    glGenTextures(1, &noiseTex);
    glBindTexture(GL_TEXTURE_3D, noiseTex);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_3D, 0);

    glGenTextures(1, &gradTex);
    glBindTexture(GL_TEXTURE_3D, gradTex);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_3D, 0);

    glGenBuffers(1, &sliceVBO);
    glGenVertexArrays(1, &sliceVAO);

    initialized = true;

    sampleResolution = 64;
    generateNoise();
}

void setFarPlane(float far) {
    defineSlicePlanes(far);
}

void defineSlicePlanes(float far) {
    // If OpenGL has not been initialized, do nothing
    if (!initialized)
        return;

    std::vector<GLfloat> data;
    numQuads = 0;

    int numSlices = far / sliceDistance - 1;
    // Add planes from farthest to nearest
    for (int i = numSlices; i > 0; i--) {
        // If you shrink this value, a border should appear.
        float size = 1;
        float z = -i * sliceDistance;
        addVector(data,  size,  size, z);
        addVector(data, -size,  size, z);
        addVector(data, -size, -size, z);
        addVector(data,  size,  size, z);
        addVector(data, -size, -size, z);
        addVector(data,  size, -size, z);
        numQuads++;
    }

    glBindBuffer(GL_ARRAY_BUFFER, sliceVBO);
    glBufferData(GL_ARRAY_BUFFER,
                 data.size() * sizeof(GLfloat),
                 data.data(),
                 GL_STATIC_DRAW);

    // Initialize VAO
    glBindVertexArray(sliceVAO);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0,
                          3,
                          GL_FLOAT,
                          GL_FALSE,
                          0,
                          nullptr);
    glBindVertexArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glUseProgram(cloudProgram);
    glUniform1f(glGetUniformLocation(cloudProgram, "layerDensity"),
                sliceDistance);
    glUseProgram(0);
}

void advanceTime(float time) {
    // Not implemented
}

void updateCameraUniforms();

void setCamera(const Camera &newCamera) {
    camera = &newCamera;
    updateCameraUniforms();
}

void updateCameraUniforms() {
    if (!initialized)
        return;

    glUseProgram(cloudProgram);

    glm::mat4 viewMatrix = camera->getViewMatrix();
    glm::mat4 invViewMatrix = glm::inverse(viewMatrix);
    glm::mat4 projMatrix = camera->getPerspectiveMatrix();
    glm::mat4 invProjMatrix = glm::inverse(projMatrix);
    glUniform3fv(glGetUniformLocation(cloudProgram, "cameraPos"),
                 1,
                 &camera->pos[0]);
    glUniformMatrix4fv(glGetUniformLocation(cloudProgram, "viewMatrix"),
                       1,
                       GL_FALSE,
                       &viewMatrix[0][0]);
    glUniformMatrix4fv(glGetUniformLocation(cloudProgram, "invViewMatrix"),
                       1,
                       GL_FALSE,
                       &invViewMatrix[0][0]);
    glUniformMatrix4fv(glGetUniformLocation(cloudProgram, "projMatrix"),
                       1,
                       GL_FALSE,
                       &projMatrix[0][0]);
    glUniformMatrix4fv(glGetUniformLocation(cloudProgram, "invProjMatrix"),
                       1,
                       GL_FALSE,
                       &invProjMatrix[0][0]);

    glUseProgram(0);
}

inline int getIndex(glm::ivec3 coord, int dimSize) {
    glm::ivec3 wrapped = {
        coord.x % dimSize,
        coord.y % dimSize,
        coord.z % dimSize
    };
    return ((wrapped[0] * dimSize) + wrapped[1]) * dimSize + wrapped[2];
}

void addGradient(int gradientResolution,
                 std::vector<glm::vec4> &texData,
                 std::vector<glm::vec3> &gradData);

void generateNoise() {
    std::vector<glm::vec4> texData(sampleResolution * sampleResolution * sampleResolution);
    // A gradient vector of the density
    std::vector<glm::vec3> gradData(sampleResolution * sampleResolution * sampleResolution);

    // For one loop,
    // First, generate all gradient vectors
    addGradient(2, texData, gradData);
    addGradient(4, texData, gradData);
    addGradient(8, texData, gradData);
    addGradient(16, texData, gradData);

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

void addGradient(int gradientResolution,
                 std::vector<glm::vec4> &texData,
                 std::vector<glm::vec3> &gradData) {

    // Generate textureSize * textureSize vec4s
    std::vector<glm::vec3> gradients(gradientResolution * gradientResolution * gradientResolution);
    initGradients(gradients);

    // At each texture coordinate, sample the gradients and compute
    //  (1) color
    //  (2) density
    //  (3) local gradient (points in direction of denser clouds)
    for (int i1 = 0; i1 < sampleResolution; i1++) {
    for (int i2 = 0; i2 < sampleResolution; i2++) {
    for (int i3 = 0; i3 < sampleResolution; i3++) {
        glm::ivec3 indexCoord(i1, i2, i3);
        int index = getIndex(indexCoord, sampleResolution);

        // Back-sampled coordinate, from 0.5 to sampleResolution - 0.5
        glm::vec3 coord = (glm::vec3(indexCoord) + glm::vec3(0.5))
                / ((float)sampleResolution);

        // Scale coordinate to gradient pixel space
        glm::vec3 gradientScaledCoord = coord * ((float)gradientResolution);
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
                    getIndex(corner + selectedCorner, gradientResolution)];
            float dot = glm::dot(localOffset, gradient);
            glm::vec3 nearness = glm::vec3(1) - glm::abs(localOffset);
            float weight = nearness.x * nearness.y * nearness.z;
            alpha += dot * weight;
            localGrad += gradient * weight;
        }
        }
        }

        glm::vec3 color = glm::vec3(0.8);
        texData[index] += glm::vec4(color, alpha) / (float) gradientResolution;
        gradData[index] += localGrad / (float) gradientResolution;
    }
    }
    }
}

void initGradients(std::vector<glm::vec3> &gradients) {
    for (size_t i = 0; i < gradients.size(); i++) {
        gradients[i] = glm::sphericalRand<GLfloat>(1);
    }
}

void renderClouds() {
    glUseProgram(cloudProgram);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_3D, noiseTex);
    glUniform1i(glGetUniformLocation(cloudProgram, "noiseTex"), 0);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_3D, gradTex);
    glUniform1i(glGetUniformLocation(cloudProgram, "gradTex"), 1);

    glBindVertexArray(sliceVAO);
    glDrawArrays(GL_TRIANGLES, 0, 6 * numQuads);
    glBindVertexArray(0);

    assert(glGetError() == 0);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_3D, 0);
    glUseProgram(0);
}


}
