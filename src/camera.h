#ifndef CAMERA_H
#define CAMERA_H
#include "utils/sceneparser.h"
#include <glm/glm.hpp>

// A class representing a virtual camera.

class Camera {
public:
    Camera();
    glm::vec3 look;
    glm::vec3 up;
    glm::vec3 pos;

    float aspectRatio;
    float heightAngle;
    float widthAngle;
    glm::mat4 viewMatrix;
    glm::mat4 camToWorldMatrix;
    glm::mat4 perspective;

    // Returns the view matrix for the current camera settings.
    glm::mat4 getViewMatrix() const;

    // Returns the height angle of the camera in RADIANS.
    float getHeightAngle() const;

    void cameraUpdate(RenderData &renderData, int width, int height);
    float getWidthAngle() const;
    glm::mat4 getPerspectiveMatrix() const;
};

#endif // CAMERA_H
