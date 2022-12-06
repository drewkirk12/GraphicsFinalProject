#include "camera.h"
#include "utils/sceneparser.h"

#include <stdexcept>

/**
 * @brief Camera::Camera
 * @param renderData - data used for camera's member variables
 * Represents camera in raytracer. Mainly computes view matrix and
 * its inverse for raytracer.cpp.
 */
Camera::Camera(const RenderData &renderData) {
    look = renderData.cameraData.look;
    up = renderData.cameraData.up;
    pos = renderData.cameraData.pos;
    heightAngle = renderData.cameraData.heightAngle;
}

/**
 * @brief Camera::getViewMatrix
 * @return view matrix
 * Code from lab. Uses look, up, pos vectors
 * to compute view matrix.
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
 * @brief Camera::getCameraToWorldMatrix
 * @return inverse of view matrix
 */
glm::mat4 Camera::getCameraToWorldMatrix() const {
    return glm::inverse(getViewMatrix());
}

/**
 * @brief Camera::getHeightAngle
 * @return height angle, to be used in raytracescene.cpp
 */
float Camera::getHeightAngle() const {
    return heightAngle;
}




