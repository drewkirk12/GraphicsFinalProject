#pragma once

#include <glm/glm.hpp>
#include "utils/sceneparser.h"

// A class representing a virtual camera.


class Camera {
public:
    Camera(const RenderData &renderData);
    glm::vec3 look;
    glm::vec3 up;
    glm::vec3 pos;
    float heightAngle;

    // Returns the view matrix for the current camera settings.
    // You might also want to define another function that return the inverse of the view matrix.
    glm::mat4 getViewMatrix() const;

    // Returns the height angle of the camera in RADIANS.
    float getHeightAngle() const;

    glm::mat4 getCameraToWorldMatrix() const;
};
