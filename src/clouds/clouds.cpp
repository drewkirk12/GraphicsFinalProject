#include "clouds.h"

#include <iostream>

#include <utils/shaderloader.h>

#include "noise.h"
#include "heightgrad.h"
#include "params.h"

namespace cloud {

bool initialized = false;

GLuint cloudProgram;

GLuint sliceVBO;
GLuint sliceVAO;
int numQuads;

GLuint noiseTex;
GLuint noiseGradTex;
GLuint heightTex;
GLuint heightGradTex;


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

    cloudProgram = ShaderLoader::createShaderProgram(
                ":/resources/shaders/cloud.vert",
                ":/resources/shaders/cloud.frag");

    glGenTextures(1, &noiseTex);
    glBindTexture(GL_TEXTURE_3D, noiseTex);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_3D, 0);

    glGenTextures(1, &noiseGradTex);
    glBindTexture(GL_TEXTURE_3D, noiseGradTex);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_3D, 0);

    glGenTextures(1, &heightTex);
    glBindTexture(GL_TEXTURE_1D, heightTex);
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_1D, 0);
    glGenTextures(1, &heightGradTex);
    glBindTexture(GL_TEXTURE_1D, heightGradTex);
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_1D, 0);


    glGenBuffers(1, &sliceVBO);
    glGenVertexArrays(1, &sliceVAO);

    initialized = true;

    generateNoise(cloudNoiseResolutions,
                  noiseSampleResolution,
                  noiseTex, noiseGradTex);

    generateHeightGradient(heightTexHeight,
                           heightTexResolution,
                           heightTex, heightGradTex);
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
    for (int i = 0; i < numSlices; i++) {
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

void renderClouds() {
    glUseProgram(cloudProgram);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_3D, noiseTex);
    glUniform1i(glGetUniformLocation(cloudProgram, "noiseTex"), 0);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_3D, noiseGradTex);
    glUniform1i(glGetUniformLocation(cloudProgram, "noiseGradTex"), 1);
    glUniform3fv(glGetUniformLocation(cloudProgram, "noiseSampleScale"),
                 1, &noiseSampleScale[0]);

    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_1D, heightTex);
    glUniform1i(glGetUniformLocation(cloudProgram, "heightTex"), 2);
    glActiveTexture(GL_TEXTURE3);
    glBindTexture(GL_TEXTURE_1D, heightGradTex);
    glUniform1i(glGetUniformLocation(cloudProgram, "heightGradTex"), 3);
    glUniform1f(glGetUniformLocation(cloudProgram, "startHeight"),
                 startHeight);
    glUniform1ui(glGetUniformLocation(cloudProgram, "heightTexHeight"),
                 heightTexHeight);

    glBindVertexArray(sliceVAO);
    // Render back to front
    for (int i = numQuads - 1; i >= 0; i--) {
        // First, render the slice into the light buffer
        glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA,
                            GL_ONE,       GL_ONE_MINUS_SRC_ALPHA);

        // Then, render the slice into the eye buffer
        // glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA,
        //                     GL_ONE_MINUS_DST_ALPHA, GL_ONE);
        glDrawArrays(GL_TRIANGLES, 6 * i, 6);
    }
    glBindVertexArray(0);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_1D, 0);
    glBindTexture(GL_TEXTURE_3D, 0);
    glUseProgram(0);
}


}
