#include "camera.h"
#include "settings.h"

/*
 * A Class representing a Camera.
 * Handles calculating the projection and view matrices
 *
 * Project 6 Notes - No changes made to this file.
 */
Camera::Camera() {}

/**
 * @brief Camera::cameraUpdate
 * @param renderData - reference to a renderData struct from realtime.cpp
 * @param width, height - dimensions of the screen to be used by camera
 */
void Camera::cameraUpdate(RenderData &renderData, int width, int height) {
    look = renderData.cameraData.look;
    up = renderData.cameraData.up;
    pos = renderData.cameraData.pos;
    heightAngle = renderData.cameraData.heightAngle;
    aspectRatio = ((float) width) / ((float) height);
    widthAngle = getWidthAngle();

    viewMatrix = getViewMatrix();
    perspective = getPerspectiveMatrix();
}



/**
 * @brief Camera::getViewMatrix - prepare and return view matrix of camera
 */
glm::mat4 Camera::getViewMatrix() const {
    glm::vec3 w = (1 / glm::length(look)) * (-1.0f * look);
    glm::vec3 v = (up - ((glm::dot(up, w) * w))) /
            glm::length(up - (glm::dot(up, w) * w));
    glm::vec3 u = glm::cross(v, w);

    glm::mat4 camRotate = glm::mat4(
                u[0], v[0], w[0], 0,
                u[1], v[1], w[1], 0,
                u[2], v[2], w[2], 0,
                0, 0, 0, 1);
    glm::mat4 camTransl = glm::mat4(
                1, 0, 0, 0,
                0, 1, 0, 0,
                0, 0, 1, 0,
                -pos[0], -pos[1], -pos[2], 1);
    glm::mat4 res = camRotate * camTransl;
    return res;
}

/**
 * @brief Camera::getWidthAngle - calculates widthAngle using given aspectRatio
 * and given heightAngle
 */
float Camera::getWidthAngle() const {
    float unitHeight = 2.0f * glm::tan(0.5f * heightAngle);
    float unitWidth = aspectRatio * unitHeight;
    float widthAngle = 2.0f * glm::atan(0.5f * unitWidth);
    return widthAngle;
}

/**
 * @brief Camera::getPerspectiveMatrix - prepares and returns projection matrix
 */
glm::mat4 Camera::getPerspectiveMatrix() const {
    float c = -(settings.nearPlane / settings.farPlane);
    float A = 1.0f / (1.0f + c);
    float B = -c / (1.0f + c);
    float fX = 1.0f / (settings.farPlane * glm::tan(0.5f * widthAngle));
    float fY = 1.0f / (settings.farPlane * glm::tan(0.5f * heightAngle));
    float fZ = 1.0f / settings.farPlane;

    glm::mat4 openGLZFix = glm::mat4( // to fix openGL's skew (from class)
                1, 0, 0, 0,
                0, 1, 0, 0,
                0, 0, -2, 0,
                0, 0, -1, 1);

    glm::mat4 mPP = glm::mat4(
                1, 0, 0, 0,
                0, 1, 0, 0,
                0, 0, A, -1,
                0, 0, B, 0);

    glm::mat4 pers = glm::mat4(
                fX, 0, 0, 0,
                0, fY, 0, 0,
                0, 0, fZ, 0,
                0, 0, 0, 1);
    glm::mat4 perspective = openGLZFix * mPP * pers;
    return perspective;
}




