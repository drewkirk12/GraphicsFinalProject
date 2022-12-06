#include "sceneparser.h"
#include "scenefilereader.h"
#include "glm/gtx/transform.hpp"

#include <chrono>
#include <memory>
#include <iostream>

/**
 * @brief SceneParser::parse
 * @param filepath - file to read scene graph from
 * @param renderData - struct to hold all render-relevant data
 * @return boolean in-case of errors
 *
 * Parses input file to get rendering data. Uses root node
 * of file's scene graph and helper function, parseHelper
 * to construct every primitive's ctm and add them to renderData as well.
 */
bool SceneParser::parse(std::string filepath, RenderData &renderData) {
    ScenefileReader fileReader = ScenefileReader(filepath);
    bool success = fileReader.readXML(); // function reads input filepath
    if (!success) {
        return false;
    }
    SceneGlobalData globalData = fileReader.getGlobalData();
    std::vector<SceneLightData> lights = fileReader.getLights();
    SceneCameraData cameraData = fileReader.getCameraData();

    renderData.globalData = globalData;
    renderData.lights = lights;
    renderData.cameraData = cameraData;

    SceneNode* root = fileReader.getRootNode();
    renderData.shapes.clear();
    glm::mat4 ctm = glm::mat4(1.0); // starting ctm, identity matrix
    parseHelper(renderData, root, ctm);
    // root now has all primitives grouped with their ctms
    return true;
}

/**
 * @brief SceneParser::parseHelper
 * @param renderData - rendering data that is being added to
 * @param root - root node of scene graph
 * @param ctm - used in recursive calls to pass ctm until
 *        leaf of graph is reached
 *  Helper function for parse function. Recursively builds ctm traveling
 *  down scene graphs and joins them with corresponding primitives to
 *  be added to renderData.shapes
 */
void SceneParser::parseHelper(RenderData &renderData,
                              SceneNode* root, glm::mat4 ctm) {
    int numTransforms = root->transformations.size();
    int numPrim = root->primitives.size();
    int numChildren = root->children.size();

    // combining transformations to form current CTM
    for (int i = 0; i < numTransforms; i++) {
        SceneTransformation* transform = root->transformations[i];
        glm::mat4 trMatrix;
        switch (transform->type) {
        case TransformationType::TRANSFORMATION_TRANSLATE:
            trMatrix = glm::translate(transform->translate);
            break;
        case TransformationType::TRANSFORMATION_SCALE:
            trMatrix = glm::scale(transform->scale);
            break;
        case TransformationType::TRANSFORMATION_ROTATE:
            trMatrix = glm::rotate(transform->angle, transform->rotate);
            break;
        case TransformationType::TRANSFORMATION_MATRIX:
            trMatrix = transform->matrix;
            break;
        }
        ctm = ctm * trMatrix;
    }
    // assigning primitives and storing in renderData
    for (int j = 0; j < numPrim; j++) {
        ScenePrimitive* prim = root->primitives[j];
        RenderShapeData render;
        render.primitive = *prim;
        render.ctm = ctm;
        renderData.shapes.push_back(render);
    }
    // recurring on children, if any
    for (int k = 0; k < numChildren; k++) {
        SceneNode* child = root->children[k];
        parseHelper(renderData, child, ctm);
    }

}


