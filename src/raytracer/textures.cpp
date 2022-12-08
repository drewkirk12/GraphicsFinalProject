#include "textures.h"
#include "raytracer/raytracescene.h"
#include "qimage.h"

Textures::Textures() {}
/**
 * Textures class holds textures-related code for Project 4.
*/

/**
 * @brief Textures::loadImages
 * @param scene - scene data
 * @param allImages - vector from raytracer.cpp, holds all RBGA vectors
 *                  that represent images
 * @param allWidths - vector from raytracer.cpp, holds all widths of
 *                  texture images
 * @param allHeights - vector from raytracer.cpp, holds all heights of
 *                  texture images
 *  Using loadIageFromFile(), loads RBGA vectors, heights, and widths of
 *  textures into global variables for raytracer.cpp.
 */
void Textures::loadImages(const RayTraceScene &scene,
                std::vector<std::vector<RGBA>> &allImages,
                std::vector<int> &allWidths, std::vector<int> &allHeights) {
    allImages.resize(20); // assuming I only really need space for 20 textures.
    allWidths.resize(20); // this was an arbitrary number I picked
    allHeights.resize(20);
    int shapeCount = 0;
    for (RenderShapeData shape : scene.getShapes()) {
        if (shape.primitive.material.textureMap.isUsed) {
            QString file = QString::fromStdString(
                        shape.primitive.material.textureMap.filename);
            int iWidth; int iHeight; std::vector<RGBA> image;
            loadImageFromFile(file, iWidth, iHeight, image);
            // image now has rgba struct made
            allImages[shapeCount] = image;
            allWidths[shapeCount] = iWidth;
            allHeights[shapeCount] = iHeight;
        }
        shapeCount++;
    }
}

/**
 * @brief Textures::loadImageFromFile
 * @param file - file of texture
 * @param width - width passed by reference to be assigned width of texture
 * @param height- height passed by reference to be assigned width of texture
 * @param image - RGBA vector to hold the image
 * Re-purposed from Projects 2D stencil code. Turns given texture into an
 * RBGA vector to be used to retrieve color values from textures.
 */
void Textures::loadImageFromFile(const QString &file, int &width,
                                 int &height, std::vector<RGBA> &image) {
    QImage myImage;
    myImage.load(file);
    myImage = myImage.convertToFormat(QImage::Format_RGBX8888);
    width = myImage.width();
    height = myImage.height();
    QByteArray arr = QByteArray::fromRawData((const char*) myImage.bits(),
                                             myImage.sizeInBytes());
    image.clear();
    image.reserve(width * height);
    for (int i = 0; i < arr.size() / 4.f; i++){
       image.push_back(RGBA{(std::uint8_t) arr[4*i], (std::uint8_t) arr[4*i+1],
                        (std::uint8_t) arr[4*i+2], (std::uint8_t) arr[4*i+3]});
    }
}

/**
 * @brief Textures::uvCoordtoImage
 * @param renderable - shape data of primitive to apply texture to
 * @param interPoint - intersection point to convert to uv, then to image
 * @param intersectionMarker - special marker I used to denote
 *                              special sides of ashape (ie. caps of cylinder)
 * @param tWidth - width of image
 * @param tHeight - height of image
 * @return index in the image's RGBA vector corresponding to
 *         intersection point.
 */
int Textures::uvCoordtoImage(RenderShapeData renderable, glm::vec3 interPoint,
                            int intersectionMarker, int tWidth, int tHeight) {
    float u; float v; int index;
    if (renderable.primitive.type == PrimitiveType::PRIMITIVE_CONE ||
            renderable.primitive.type == PrimitiveType::PRIMITIVE_CYLINDER) {
        if (intersectionMarker >= 2) { // indicates one of the caps
          std::tuple<float, float> uv = planeUV(interPoint[0], interPoint[2]);
            u = get<0>(uv);
            v = get<1>(uv);
        }
        else {// body was hit
            u = perimeterU(interPoint[0], interPoint[2]);
            v = interPoint[1] + 0.5f;
        }
    }
    else if (renderable.primitive.type == PrimitiveType::PRIMITIVE_SPHERE) {
        u = perimeterU(interPoint[0], interPoint[2]);
        v = sphereV(interPoint[1]);
    }
    else { // cube
        std::tuple<float, float> uv;
        if (intersectionMarker == 0) {
            uv = planeUV(-interPoint[2], interPoint[1]);
        }
        if (intersectionMarker == 1) {
            uv = planeUV(interPoint[2], interPoint[1]);
        }
        if (intersectionMarker == 2) {
            uv = planeUV(interPoint[0], -interPoint[2]);
        }
        if (intersectionMarker == 3) {
            uv = planeUV(interPoint[0], interPoint[2]);
        }
        if (intersectionMarker == 4) {
            uv = planeUV(interPoint[0], interPoint[1]);
        }
        if (intersectionMarker == 5) {
            uv = planeUV(-interPoint[0], interPoint[1]);
        }
        u = get<0>(uv); v = get<1>(uv);
    }
    int rU = renderable.primitive.material.textureMap.repeatU;
    int rV = renderable.primitive.material.textureMap.repeatV;
    int col; int row;
    // turning uv coordinate sinto row, col coordinates in image
    if (rU != 1 || rV != 1) {
        index= uvMulti(u, v, tWidth, tHeight, rU, rV);
    }
    else {
        index = uvToRowCol(u, v, tWidth, tHeight);
    }
    return index;
}

/**
 * @brief Textures::uvToRowCol
 * @param u, v - uv coordinates
 * @param textWidth, textureHeight - width & height of image
 * @return index of RGBA vector of image
 * Helper function to convert uv coordinates to row, col for
 * a non-tiled texture.
 */
int Textures::uvToRowCol(float u, float v, int textWidth, int textureHeight) {
    int boundsCheck = 0;
    u = std::fmin(u, 1.0f);
    v = std::fmax(v, 0.0f);
    if (u > 1.0f || v < 0.0f) {
        boundsCheck = 1;
    }
    int col = std::fmax(std::floor(u * textWidth), 0);
    int row = std::floor(std::fmax((1 - v), 0) * textureHeight);
    return ((textWidth * row) + col) - boundsCheck;
}

/**
 * @brief Textures::uvMulti
 * @param u, v - uv coordinates
 * @param textWidth, textHeight - width and height of image
 * @param repeatU - number of times to tile texture horizontallly
 * @param repeatV - number of times to tile texture vertically
 * @return index of RGBA vector of image
 * Helper function to convert uv coordinates to row, col for
 * a tiled texture.
 */
int Textures::uvMulti(float u, float v, int textWidth, int textHeight,
                                           int repeatU, int repeatV) {
    int boundsCheck = 0;
    u = std::fmin(u, 1.0f);
    v = std::fmax(v, 0.0f);
    if (u > 1.0f || v < 0.0f) {
        boundsCheck = 1;
    }
    int col = (int) std::floor(u * repeatU * textWidth) % textWidth;
    int row = (int) std::floor((1 - v) * repeatV * textHeight) % textHeight;
    return ((textWidth * row) + col) - boundsCheck;
}

/**
 * @brief Textures::planeUV
 * @param px, pz - relevant cartesian coordinates to use
 * @return  uv coordinates from px, pz mapped to uv unit square
 */
std::tuple<float, float> Textures::planeUV(float px, float pz) {
    return std::make_tuple(px + 0.5f, pz + 0.5f); // for caps and square faces
}
/**
 * @brief Textures::perimeterU
 * @param px, pz - relevant cartesian coordinates to use
 * @return "u" value using perimeter of circle, cylinder, or cone
 */
float Textures::perimeterU(float px, float pz) {
    if (px != 0.f && pz != 0.f) { // avoids divide by 0 + for poles of sphere
        float theta = glm::atan(pz, px);
        if (theta < 0.f) {
            return (-theta) / (2.0f * M_PI);
        }
        return 1.0f - (theta / (2.0f * M_PI));
    }
    return 0.5f;
}

/**
 * @brief Textures::sphereV
 * @param py - relevant cartesian coordinate
 * @return "v" value of sphere using its py coordinate
 */
float Textures::sphereV(float py) {
    float phi = glm::asin(py / 0.5f); // unit sphere radius
    return (phi / M_PI) + 0.5f;
}

