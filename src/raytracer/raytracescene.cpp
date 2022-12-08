#include "camera/camera.h"
#include "raytracescene.h"
#include "utils/sceneparser.h"

#include <stdexcept>
#include <cmath>

/**
 * @brief RayTraceScene::RayTraceScene
 * @param width, height - canvas height and width
 * @param metaData - renderData passed prepared in sceneparser.cpp
 * Represents a scene to be ray-traced. Has getters that are used
 * in raytracer.cpp to access rendering data. Constructor
 * sets member variables from metaData.
 */
RayTraceScene::RayTraceScene(int width, int height,
                             const RenderData &metaData) : camera{metaData} {
    aspectRatio = (float) width / (float) height;
    cWidth = width;
    cHeight = height;
    shapes = metaData.shapes;
    lights = metaData.lights;
    globalData = metaData.globalData;
    viewMatrix = camera.getViewMatrix();
}

/**
 * @brief RayTraceScene::computeRayDir
 * @param row, col - position on view plane
 * @return - ray direction vector
 * Computes ray direction for any given row, col on the view plane.
 */
const glm::vec3 RayTraceScene::computeRayDir(int row, int col) const {
    // we are in camera space -> Camera position: (0,0,0)
    float x = ((col + 0.5) / cWidth) - 0.5;
    float y = ((cHeight - 0.5 - row) / cHeight) - 0.5;
    float k = 1; // assumed to be one

    float u = getPlaneWidth() * x;
    float v = getPlaneHeight() * y;

    glm::vec3 dir{u, v, -k};
    return dir;
}

/**
 * @brief RayTraceScene::getShapes
 * @return renderData's shapes vector to be used in raytracer
 */
const std::vector<RenderShapeData> RayTraceScene::getShapes() const {
    return shapes;
}

/**
 * @brief RayTraceScene::getLights
 * @return renderData's lights vector to be used in raytracer
 */
const std::vector<SceneLightData> RayTraceScene::getLights() const {
    return lights;
}

/**
 * @brief RayTraceScene::getGlobalData
 * @return renderData's globalData struct (ie. ka, ks, etc.)
 */
const SceneGlobalData& RayTraceScene::getGlobalData() const {
    return globalData;
}

/**
 * @brief RayTraceScene::getCamera
 * @return - camera, to use it and its methods in raytracer
 */
const Camera& RayTraceScene::getCamera() const {
    return camera;
}

/**
 * @brief RayTraceScene::getPlaneHeight
 * @return - calculates plane height using height angle
 */
const float RayTraceScene::getPlaneHeight() const {
    float hAngle = camera.getHeightAngle();
    return 2 * std::tan(hAngle / 2);
}

/**
 * @brief RayTraceScene::getPlaneWidth
 * @return  - calculates plane width from aspect ratio and height
 */
const float RayTraceScene::getPlaneWidth() const {
    return (getPlaneHeight() * aspectRatio);
}

/**
 * @brief RayTraceScene::getCanvasWidth
 * @return - gets canvas width
 */
const int RayTraceScene::getCanvasWidth() const {
    return cWidth;
}

/**
 * @brief RayTraceScene::getCanvasHeight
 * @return - gets canvas height
 */
const int RayTraceScene::getCanvasHeight() const {
    return cHeight;
}




