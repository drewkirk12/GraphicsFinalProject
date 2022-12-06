#include "raytracer.h"
#include "raytracescene.h"
#include "ray.h"

#include <iostream>
#include <vector>
#include <cmath>
#include <tuple>

/**
 * @brief RayTracer::RayTracer
 * @param config - configurations from .ini file
 * RayTracer Class. Represents a raytracer that renders images using
 * methods prepared by RayTraceScene.
 */
RayTracer::RayTracer(Config config): m_config(config) {}

/**
 * @brief RayTracer::render
 * @param imageData - RGBA values to be found to render image
 * @param scene - RayTraceScene prepared for rendering
 * Renders scene by:
 * 1. Looping through each cell in view plane
 * 2. For each cell computing a ray in world space
 * 3. Looping through each shape to be rendered -> with traceRay() method
 * 4. transfering the ray to object space
 * 5. Using ray.cpp to check for intersections using implict functions
 * 6. From closest object (if any) to render, get object space normal and
 *  intersection point and convert back to world space
 * 7. Finally calling phong function to find proper pixel colors
 *    using world space normal and world space intersection.
 *
 *  EDIT: now in Illuminate, the function also loads in textures
 *  using methods from the Textures class.
 */
void RayTracer::render(RGBA *imageData, const RayTraceScene &scene) {
    float k = 1; // assumption
    int W = scene.getCanvasWidth(), H = scene.getCanvasHeight();
    if (m_config.enableTextureMap) {
        textures.loadImages(scene, allImages, allWidths, allHeights);
    }
    // looping through each cell
    for (int i = 0; i < H; i++) { // row
        for (int j = 0; j < W; j++) { // col
            // computing ray in world space
            glm::vec3 d = scene.computeRayDir(i, j); // in camera space
            // convert from camera to world:
            glm::vec4 eye{0, 0, 0, 1};
            glm::vec4 dir{d, 0};
            eye = scene.getCamera().getCameraToWorldMatrix() * eye;
            dir = scene.getCamera().getCameraToWorldMatrix() * dir;
            dir = glm::normalize(dir);

            glm::vec4 colorVec = traceRay(eye, dir, scene, 0, 0, 0);
            RGBA color = toRGBA(colorVec);
            int imageIndex = (W * i) + j;
            imageData[imageIndex].r = color.r;
            imageData[imageIndex].g = color.g;
            imageData[imageIndex].b = color.b;
        }
    }
}

/**
 * @brief RayTracer::traceRay
 * @param start - starting point of ray (camera eye or intersectionn point)
 * @param dir - direction of ray
 * @param scene - scene data
 * @param depth - depth of recursion; for recursive calls
 *              -> passed into phong() where it's checked
 *                 to prevent infinite recursive calls
 * @param epsilon - for recursive calls, offset to ensure no spurious
 *                  intersection with the surface itself
 * @param recurOffset - very similar to epsilon; look at README for more info
 * @return - vec4 of total color for the pixel intersected by the ray or
 *           for color contribution of a reflected surface
 *  The trace ray function finds the intersections between the ray and objects
 *  using methods from the Ray class. It also finds potential color contributions
 *  from texture maps.
 */
glm::vec4 RayTracer::traceRay(glm::vec3 start, glm::vec3 dir,
                              const RayTraceScene &scene, int depth,
                              float epsilon, float recurOffset) {
    glm::vec3 startEp = {start[0] + epsilon * dir[0],
                         start[1] + epsilon * dir[1],
                         start[2] + epsilon * dir[2]};
    glm::vec4 start4{startEp, 1};
    glm::vec4 dir4{dir, 0};

    float finalT = INFINITY;
    RenderShapeData renderable;
    glm::vec3 interPoint;
    glm::vec3 norm;
    int intersectionMarker; // marker indicates special points (ie. cone caps)
    int shapeID; // id for each object. Used to retrieve textures
    int shapeCount = 0;
    std::tuple<float, glm::vec3, glm::vec3, int> infoTuple;
    // infoTuple will be returned from ray.intersection
    for (RenderShapeData shape : scene.getShapes()) {
        // convert ray to object space
        glm::vec4 objectStart = glm::inverse(shape.ctm) * start4;
        glm::vec4 objectDir = glm::inverse(shape.ctm) * dir4;

        Ray ray{objectStart, objectDir};
        // checking for intersections using ray.cpp
        infoTuple = ray.intersectionCheck(shape.primitive.type);
        float tV = get<0>(infoTuple);
        if (tV < finalT && tV > recurOffset) {
            finalT = tV;
            renderable = shape;
            shapeID = shapeCount; // current nearest shape ID recorded
            intersectionMarker = get<3>(infoTuple); // marker recorded
            norm = get<1>(infoTuple); // currently in object space
            interPoint = get<2>(infoTuple); // currently in object space
        }
        // if finalT stays infinite, nothing to be rendered
        shapeCount++; // increment counter for next shape
    }
    if (finalT != INFINITY) {
        glm::mat4 ctm = renderable.ctm;
        glm::mat3 upperCTM = glm::mat3(ctm);
        upperCTM = glm::transpose(upperCTM);
        upperCTM = glm::inverse(upperCTM);
        glm::vec4 inter4{interPoint, 1};
        glm::vec4 worldInter = ctm * inter4; // back to world space
        glm::vec3 worldNorm = upperCTM * norm; // back to world space
        worldNorm = glm::normalize(worldNorm);

        // preparing arguments for phong
        glm::vec4 dirToCam4 = start4 - worldInter;
        glm::vec3 dirToCam{dirToCam4[0], dirToCam4[1], dirToCam4[2]};
        dirToCam = glm::normalize(dirToCam);

        RGBA textureVec{0, 0, 0, 0}; // checking for textures
        if (renderable.primitive.material.textureMap.isUsed &&
                                            m_config.enableTextureMap) {
            int index = textures.uvCoordtoImage(renderable, interPoint,
                                intersectionMarker, allWidths[shapeID],
                                                   allHeights[shapeID]);
            textureVec = allImages[shapeID][index];
        }

        // checking epsilon & depth for recursive calls
        if (epsilon != 0.0f) {
            depth++; // depth of recursion
        } // calling phong
        return phong(worldInter, worldNorm,
                           dirToCam, renderable.primitive.material, scene,
                                                        depth, textureVec);
    }
    glm::vec4 none{0.0, 0.0, 0.0, 1.0};
    return none;
}

/**
 * @brief RayTracer::shadowCheck
 * @param interPoint - intersection point of initial ray
 * @param dir - direction toward the light source
 * @param lightPos - position of light (all infinity for directional)
 * @param scene - scene data
 * @return - boolean: 1 if a shadow will appear, o/w 0.
 * Shadow check performs a piece of the raytrace() function,
 * but looks up until any surface  is intersected between itself and the
 * light source. Returns boolean to indicate if shadow will be cast.
 * Called in the phong() method.
 */
int RayTracer::shadowCheck(glm::vec3 interPoint, glm::vec3 dir,
                           glm::vec3 lightPos, const RayTraceScene &scene) {
    // I surprisingly found a negative epsilon to work.
    float ep = -0.005;
    glm::vec3 interPointEp = {interPoint[0] + ep * dir[0],
                              interPoint[1] + ep * dir[1],
                              interPoint[2] + ep * dir[2]};
    glm::vec4 interPoint4{interPointEp, 1};
    glm::vec4 dir4{dir, 0}; // world matrix
    float lightT;
    // check for directional light source which should have INFINITY
    if (lightPos[0] == INFINITY) {
        lightT = INFINITY;
    }
    else {
        lightT = glm::distance(interPointEp, lightPos);
    }
    float finalT = lightT;
    for (RenderShapeData shape : scene.getShapes()) {
        glm::vec4 objectInter = glm::inverse(shape.ctm) * interPoint4;
        glm::vec4 objectDir = glm::inverse(shape.ctm) * dir4;
        // ray in object space
        Ray ray{objectInter, objectDir};
        std::tuple<float, glm::vec3, glm::vec3, int> tup =
                ray.intersectionCheck(shape.primitive.type);
        float tV = get<0>(tup);
        if (tV < finalT && tV > 0.0055f) {
            return 1; // intersected with another object
        }
    }
    return 0;
}

/**
 * @brief RayTracer::phong
 * @param interPos, norm - in world space, intersection pos. and surface normal
 * @param dirToCam - direction vector toward camera from intersection
 * @param material - material of object intersected
 * @param scene - RayTraceScene to get information from
 * @return RGBA values
 * Phong Illumination model is carried out here. Uses object material
 * and object positioning relative to camera and light to render
 * object.
 * Directional, Spot, and Point lights implemented with varying set-up done
 * at the top of the function for each.
 * Shadows are checked for by shadowCheck() and if found, result in the
 * current light to be skipped.
 * Diffuse (w/ blending for textures) and specular lighting are handled here.
 * Finally, reflections are handled at the end, which call traceRay() to keep
 * finding intersections for reflections.
 */
glm::vec4 RayTracer::phong(glm::vec3 interPos, glm::vec3  norm,
                           glm::vec3  dirToCam, SceneMaterial material,
                         const RayTraceScene &scene, int depth,RGBA texture) {
    // illumination vector will maintain cumulative rgb values
    glm::vec4 illumination(0.0, 0.0, 0.0, 1.0);
    glm::vec3 textureVec{(float) texture.r / 255.f,
                (float) texture.g / 255.f,
                (float) texture.b / 255.f};

    if (depth > 4) {return illumination;} // maximum depth

    // setting up local variables to decrease clutter below
    float ka = scene.getGlobalData().ka;
    float kd = scene.getGlobalData().kd;
    float ks = scene.getGlobalData().ks;
    std::vector<SceneLightData> lights = scene.getLights();

    // Ambient Term
    illumination[0] += (ka * material.cAmbient[0]);
    illumination[1] += (ka * material.cAmbient[1]);
    illumination[2] += (ka * material.cAmbient[2]);

    for (const SceneLightData &light : lights) {
        float fatt = 1.0;
        glm::vec3 L_i;
        float dotP;
        glm::vec4 intensVec = light.color;
        glm::vec3 lightPos;

        // Setting up L_i, dot products, etc. for different kinds of lights
        if (light.type == LightType::LIGHT_DIRECTIONAL) {
            lightPos = {INFINITY, INFINITY, INFINITY};
            // no such light position, but used in shadowCheck
            L_i = {light.dir[0], light.dir[1], light.dir[2]};
            L_i = -glm::normalize(L_i); // makes sure it points toward light
        }
        else {
            lightPos = {light.pos[0], light.pos[1], light.pos[2]};
            float dist = glm::distance(lightPos, interPos);
            float c1 = light.function[0];
            float c2 = light.function[1];
            float c3 = light.function[2];
            fatt = (float) fmin(1.0f, (1.0f / (float) (c1 + (dist * c2) +
                                           ((float) std::pow(dist, 2) * c3))));
            L_i = glm::normalize(lightPos - interPos);
            // gets direction vector, normalize it

            if (light.type == LightType::LIGHT_SPOT) {
                float thetaO = light.angle;
                float thetaI = light.angle - light.penumbra;
                glm::vec3 lightDir{light.dir[0], light.dir[1], light.dir[2]};
                // preparing intensity vector with spot light
                float currX = angleBTWVectors(-L_i, lightDir);
                for (int i = 0; i < 3; i++) {
                    if (thetaI < currX && currX <= thetaO) {
                        intensVec[i] = (1.0 - falloff(currX, thetaI, thetaO)
                                                            * intensVec[i]);
                    }
                    else if (currX > thetaO) {
                        continue; // outside penumbra
                    }
                }
            }
         }

        if(m_config.enableShadow && shadowCheck(interPos,
                                    L_i, lightPos, scene)) {
            continue; // continues loop with next light if shadow detected
        }

        // Set-up for Diffuse Term
        dotP = dot(norm, L_i);

        // Set-up for Specular Term
        glm::vec3 R_i = glm::normalize(glm::reflect(-L_i, norm));
        float dotP2 = glm::dot(R_i, dirToCam);
        if (dotP2 >= 0.0) { // clamping here to avoid (-n)^0 = 1
            dotP2 = (float) pow(dotP2, material.shininess);
        }

        for (int i = 0; i < 3; i++) {
            float diffuseBlend; // interpolation for textures
            float A = (kd * material.cDiffuse[i]);
            float B = textureVec[i];
            diffuseBlend = A + (material.blend * (B - A));

            float diffIntens = diffuseBlend * fatt * intensVec[i] * dotP;
            illumination[i] += std::fmax(0.0f, diffIntens);
            float specIntens = fatt * intensVec[i] *
                    ks * material.cSpecular[i] * dotP2;
            illumination[i] += std::fmax(0.0f, specIntens);
        }
    }

    // reflection call & check if enabled in config
    if (glm::length(material.cReflective) > 0 && m_config.enableReflection) {
        glm::vec3 reflectDir = glm::normalize(glm::reflect(-dirToCam, norm));
        glm::vec4 reflection = traceRay(interPos, reflectDir,
                                        scene, depth, -0.005f, 0.05f);
        illumination[0] += material.cReflective[0] * ks * reflection[0];
        illumination[1] += material.cReflective[1] * ks * reflection[1];
        illumination[2] += material.cReflective[2] * ks * reflection[2];
    }
    return illumination;
}

/**
 * @brief RayTracer::toRGBA
 * @param illumination - vector of illumination values to convert
 * @return RGBA conversion of illumination values
 */
RGBA RayTracer::toRGBA(const glm::vec4 &illumination) {
    RGBA res;
    res.r = (std::uint8_t) 255 * fmin((fmax(illumination[0], 0)), 1);
    res.g = (std::uint8_t) 255 * fmin((fmax(illumination[1], 0)), 1);
    res.b = (std::uint8_t) 255 * fmin((fmax(illumination[2], 0)), 1);
    return res;
}

/**
 * @brief RayTracer::falloff
 * @param angleX - angle away from light of intersection point
 * @param theta_I - inner angle of spot light
 * @param theta_O - outer angle of spot light
 * @return - falloff-value to calculate how much light intensity should diminish
 * Helper function for spot lights. Calculates falloff.
 */
float RayTracer::falloff(float angleX, float theta_I, float theta_O) {
    float term= (angleX - theta_I) / (theta_O - theta_I);
    float res = -2.0 * (float) std::pow(term, 3) + 3.0 * (float) std::pow(term, 2);
    return res;
}
/**
 * @brief RayTracer::angleBTWVectors
 * @param ang1, ang2 -> given vectors
 * @return angle between two vectors
 * Helper function used in calculating angle between point
 * of intersection and light direction for spot lights.
 */
float RayTracer::angleBTWVectors(glm::vec3 ang1, glm::vec3 ang2) {
    float dot = glm::dot(ang1, ang2);
    return glm::acos(dot);
}





