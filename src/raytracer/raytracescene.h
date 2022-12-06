#pragma once

#include "utils/scenedata.h"
#include "utils/sceneparser.h"
#include "camera/camera.h"

// A class representing a scene to be ray-traced

class RayTraceScene {
public:
    RayTraceScene(int width, int height, const RenderData &metaData);
    // my member variables
    Camera camera;
    float aspectRatio;
    int cWidth;
    int cHeight;
    SceneGlobalData globalData;
    glm::mat4 viewMatrix;
    std::vector<RenderShapeData> shapes;
    std::vector<SceneLightData> lights;

    // The getter of the width of the scene
    const int& width() const;

    // The getter of the height of the scene
    const int& height() const;

    // The getter of the global data of the scene
    const SceneGlobalData& getGlobalData() const;

    // The getter of the shared pointer to the camera instance of the scene
    const Camera& getCamera() const;

    // my functions
    const float getPlaneHeight() const;
    const float getPlaneWidth() const;
    const glm::vec3 computeRayDir(int i, int j) const;
    const int getCanvasWidth() const;
    const int getCanvasHeight() const;
    const std::vector<RenderShapeData> getShapes() const;
    const std::vector<SceneLightData> getLights() const;
};
