#include "ray.h"
#include "utils/scenedata.h"

#include <tuple>
#include <cmath>
#include <vector>
#include <algorithm>

/**
 * @brief Ray::Ray
 * @param eye - position of camera in object space
 * @param d - direction of ray in object space
 * Ray class is used to calculate intersections
 * through implicit function definitions of primitives.
 * It is used with rays defined in object space to work with primitives that
 * have undergone transformations. Functions here also return surface normals
 * for illumination done in raytracer.cpp
 */
Ray::Ray(glm::vec4 eye, glm::vec4 d) {
    // preparing member variables to make calculations look cleaner below
    px = eye[0]; py = eye[1]; pz = eye[2];
    dx = d[0]; dy = d[1]; dz = d[2];
}

/**
 * @brief Ray::quadHelper
 * @param a, b, c - variables of the quadratic formula
 * @return - t1, t2 values corresponding to solved quadratic form values
 * Helper function called only when it is known quadratic formula
 * will result in two values.
 */
std::tuple<float, float> Ray::quadHelper(float a, float b, float c) {
    float t1 = (-b + std::sqrt(pow(b, 2) - (4 * a * c))) / (2 * a);
    float t2 = (-b - std::sqrt(pow(b, 2) - (4 * a * c))) / (2 * a);
    return std::make_tuple(t1, t2);
}

/**
 * @brief Ray::discrimCheck
 * @param a, b, c - variables of discriminant
 * @return true if discriminant is positive
 * indicating that quad. formula will output two values
 */
bool Ray::discrimCheck(float a, float b, float c) {
    return ((pow(b, 2) - (4 * a * c)) > 0);
}
/**
 * @brief Ray::planeIntersect
 * @param A, B, C, D - coefficients of plane implicit form
 * Ax + By + Cz + D = 0
 * @return t-value (if any) for which ray intersects plane
 * returns infinity otherwise.
 */
float Ray::planeIntersect(float A, float B, float C, float D) {
    float check = A*dx + B*dy + C*dz;
    if (check == 0) {return INFINITY;} // no intersection, toss value
    else {
        float t = (-1.0 * (A*px + B*py + C*pz + D)) / check;
        float ret = INFINITY;
        if (t > 0) { // toss 0 or negative t-values
            return t;
        }
        return ret;
    }
}

/**
 * @brief Ray::cubeIntersect
 * @return 1. t-value for which cube is intersected
 *         2. Surface Normal Vector
 *         3. Intersection point
 * Checks for cube intersection on any of its 6 planes
 * using the unit cube (1x1x1) centered at origin in object space.
 */
std::tuple<float, glm::vec3, glm::vec3, int> Ray::cubeIntersect() {
    // repeated testing for t-values to compare
    float t1 = planeIntersect(2, 0, 0, -1); // for x = 0.5
    float y1 = py + t1*dy, z1 = pz + t1*dz;
    if (y1 > 0.5 || y1 < -0.5 || z1 > 0.5 || z1 < -0.5) {t1 = INFINITY;}

    float t2 = planeIntersect(-2, 0, 0, -1); // for x = -0.5
    float y2 = py + t2*dy, z2 = pz + t2*dz;
    if (y2 > 0.5 || y2 < -0.5 || z2 > 0.5 || z2 < -0.5) {t2 = INFINITY;}

    float t3 = planeIntersect(0, 2, 0, -1); // for y = 0.5
    float x3 = px + t3*dx, z3 = pz + t3*dz;
    if (x3 > 0.5 || x3 < -0.5 || z3 > 0.5 || z3 < -0.5) {t3 = INFINITY;}

    float t4 = planeIntersect(0, -2, 0, -1); // for y = -0.5
    float x4 = px + t4*dx, z4 = pz + t4*dz;
    if (x4 > 0.5 || x4 < -0.5 || z4 > 0.5 || z4 < -0.5) {t4 = INFINITY;}

    float t5 = planeIntersect(0, 0, 2, -1); // for z = 0.5
    float x5 = px + t5*dx, y5 = py + t5*dy;
    if (x5 > 0.5 || x5 < -0.5 || y5 > 0.5 || y5 < -0.5) {t5 = INFINITY;}

    float t6 = planeIntersect(0, 0, -2, -1); // for z = -0.5
    float x6 = px + t6*dx, y6 = py + t6*dy;
    if (x6 > 0.5 || x6 < -0.5 || y6 > 0.5 || y6 < -0.5) {t6 = INFINITY;}
    // find smallest, non-negative t-value
    std::vector<float> tVals{t1, t2, t3, t4, t5, t6};
    int marker;
    float t = INFINITY;
    for (int i = 0; i < 6; i++) {
        if (tVals[i] < t) {
            t = tVals[i];
            marker = i;
        }
    }
    if (t != INFINITY) {
        float xi = px + t*dx;
        float yi = py + t*dy;
        float zi = pz + t*dz;
        glm::vec3 interPoint{xi, yi, zi};
        glm::vec3 norm;
        switch (marker) { // based on which t-value was chosen, set normal
            case 0: norm = {1, 0, 0};  break;
            case 1: norm = {-1, 0, 0}; break;
            case 2: norm = {0, 1, 0};  break;
            case 3: norm = {0, -1, 0}; break;
            case 4: norm = {0, 0, 1};  break;
            case 5: norm = {0, 0, -1}; break;
        }
        norm = glm::normalize(norm);
        return std::make_tuple(t, norm, interPoint, marker);
    }
    return std::make_tuple(INFINITY, glm::vec3{0}, glm::vec3{0}, 0);
    // if no intersection found
}

/**
 * @brief Ray::coneIntersect
 * @param t1, t2 - t-values from quadratic formula
 * @return tuple containing:
 *         1. t-value for which cone is intersected
 *         2. Surface Normal Vector
 *         3. Intersection point
 *  Function finds t-values (if any) that intersect unit cone (r=0.5, h = 1)
 *  centered at origin in object space. Also, checks for intersection with its
 *  circular base, returning corresponding t-values, if they were intersected.
 */
std::tuple<float, glm::vec3, glm::vec3, int> Ray::coneIntersect(float t1, float t2) {
    float y1 = py + t1*dy;
    float y2 = py + t2*dy;
    if (y1 > 0.5 || y1 < -0.5) {t1 = INFINITY;} // tossed
    if (y2 > 0.5 || y2 < -0.5) {t2 = INFINITY;} // tossed

    // checking intersection with base
    float t3 = planeIntersect(0, -2, 0, -1); // for plane y = -0.5
    float x3 = px + t3*dx, z3 = pz + t3*dz;
    float circInter = std::pow(x3, 2) + std::pow(z3, 2);
    if (circInter > 0.25) {t3 = INFINITY;} // tossed

    // finding smallest, non-negative t-value
    glm::vec3 tVals{t1, t2, t3};
    int marker = 3;
    float t = INFINITY;
    for (int i = 0; i < 3; i++) {
        if (tVals[i] < t) {
            t = tVals[i];
            marker = i; // marker used to identify which t-value works
        }
    }
    if (t != INFINITY) {
        float xi = px + t*dx;
        float yi = py + t*dy;
        float zi = pz + t*dz;
        glm::vec3 interPoint{xi, yi, zi};
        glm::vec3 norm;
        if (marker == 0 || marker == 1) {
            // setting normal using gradient of implicit function
            norm = {(2.0 * xi), (((-0.5) * yi) + (0.25)), (2.0 * zi)};
        }
        else { // normal for circular base
            norm = {0, -1, 0};
        }
        norm = glm::normalize(norm);
        return std::make_tuple(t, norm, interPoint, marker);
    }
    return std::make_tuple(INFINITY, glm::vec3{0}, glm::vec3{0}, 0);
    // means no intersection
}

/**
 * @brief Ray::cylinderIntersect
 * @param t1, t2 - values found through quadratic formula
 * @return tuple containing:
 *         1. t-value for which cylinder is intersected
 *         2. Surface Normal Vector
 *         3. Intersection point
 *  Function finds t-values (if any) that intersect unit cylinder (r=0.5, h = 1)
 *  centered at origin in object space. Also, checks for intersection with its
 *  circular base and top, returning corresponding t-values,
 *  if they were intersected.
 */
std::tuple<float, glm::vec3, glm::vec3, int> Ray::cylinderIntersect(
        float t1, float t2) {
    // check cylinder body
    float y1 = py + t1*dy;
    float y2 = py + t2*dy;
    if (y1 > 0.5 || y1 < -0.5) {t1 = INFINITY;} // tossed
    if (y2 > 0.5 || y2 < -0.5) {t2 = INFINITY;} // tossed

    // top cap
    float t3 = planeIntersect(0, 2, 0, -1); // for y = 0.5
    float x3 = px + t3*dx, z3 = pz + t3*dz;
    float circInter = std::pow(x3, 2) + std::pow(z3, 2);
    if (circInter > 0.25) {t3 = INFINITY;} // tossed

    // bottom cap
    float t4 = planeIntersect(0, -2, 0, -1); // for y = -0.5
    float x4 = px + t4*dx, z4 = pz + t4*dz;
    circInter = std::pow(x4, 2) + std::pow(z4, 2);
    if (circInter > 0.25) {t4 = INFINITY;} // tossed

    // determine smallest, non-negative t-value
    glm::vec4 tVals{t1, t2, t3, t4};
    int marker = 4;
    float t = INFINITY;
    for (int i = 0; i < 4; i++) {
        if (tVals[i] < t) {
            t = tVals[i];
            marker = i; // marker indicates which of them was chosen below
        }
    }
    if (t != INFINITY) {
        float xi = px + t*dx;
        float yi = py + t*dy;
        float zi = pz + t*dz;
        glm::vec3 interPoint{xi, yi, zi};
        glm::vec3 norm;
        if (marker == 0 || marker == 1) {
            // surface normal found through gradient of implicit function
            norm = {(2*xi), 0, (2*zi)};
        }
        else if (marker == 2) {
            norm = {0, 1, 0}; // normal for top cap
        }
        else {
            norm = {0, -1, 0}; // normal for bottom cap
        }
        norm = glm::normalize(norm);
        return std::make_tuple(t, norm, interPoint, marker);
    }
    return std::make_tuple(INFINITY, glm::vec3{0}, glm::vec3{0}, 0);
    // means no intersection
}
/**
 * @brief Ray::intersectionCheck
 * @param type - type of primitive to check
 * @return - tuple containing:
 *         1. t-value for which object is intersected
 *         2. Surface Normal Vector
 *         3. Intersection point
 *  Main function of ray. Called from raytracer.cpp. Determines
 *  t-value (if any) of objects intersected by the ray.
 *  Calls cylinder, cone, and cube helper functions which
 *  deal with implicit functios for those shapes. Primitive spheres are
 *  dealt with in this function. Quadratic formula helper functions
 *  are also used to determine early if shapes are intersected or not.
 */
std::tuple<float, glm::vec3, glm::vec3, int> Ray::intersectionCheck(
        PrimitiveType type) {
    float a = 0, b = 0, c = 0;
    // determine implicit functions for quad. formula
    switch (type) {
        case PrimitiveType::PRIMITIVE_SPHERE:
            a = (std::pow(dx, 2) + std::pow(dy, 2) + std::pow(dz, 2));
            b = ((2 * px * dx) + (2 * py * dy) + (2 * pz * dz));
            c = (std::pow(px, 2) + std::pow(py, 2) + std::pow(pz, 2) - 0.25);
            break;
        case PrimitiveType::PRIMITIVE_CYLINDER:
            a = (std::pow(dx, 2) + std::pow(dz, 2));
            b = ((2 * px * dx) + (2 * pz * dz));
            c = (std::pow(px, 2) + std::pow(pz, 2) - 0.25);
        break;
        case PrimitiveType::PRIMITIVE_CONE:
            a = (std::pow(dx, 2) + std::pow(dz, 2) - (0.25 * std::pow(dy, 2)));
            b = ((2 * px * dx) + (2 * pz * dz) - (0.5) * py * dy + 0.25 * dy);
            c = (std::pow(px, 2) + std::pow(pz, 2) - (0.25) *
                 std::pow(py, 2) + (0.25) * py - (0.0625));
            break;
        case PrimitiveType::PRIMITIVE_CUBE:
            // cube does not need quad. formula. Go straight to its helper
            return cubeIntersect();
        case PrimitiveType::PRIMITIVE_MESH:
        case PrimitiveType::PRIMITIVE_TORUS:
            break;
    }
    if (discrimCheck(a, b, c)) { // checks for positive discriminant
        std::tuple<float, float> tVal = quadHelper(a, b, c);
        float t1 = get<0>(tVal);
        float t2 = get<1>(tVal);
        float t = 0;
        switch (type) {
            // implicit function of sphere
            case PrimitiveType::PRIMITIVE_SPHERE:
                if (t1 <= 0) {t1 = INFINITY;} // toss
                if (t2 <= 0) {t2 = INFINITY;} // toss
                t = std::fmin(t1, t2);
                if (t != INFINITY) {
                    float xi = px + t*dx;
                    float yi = py + t*dy;
                    float zi = pz + t*dz;
                    glm::vec3 interPoint{xi, yi, zi};
                    glm::vec3 norm{(2*xi), (2*yi), (2*zi)};
                    // surface normal from gradient of implicit function
                    norm  = glm::normalize(norm);
                    return std::make_tuple(t, norm, interPoint, 0);
                }
            case PrimitiveType::PRIMITIVE_CYLINDER:
                return cylinderIntersect(t1, t2);
                break;
            case PrimitiveType::PRIMITIVE_CONE:
                return coneIntersect(t1, t2);
                break;
            default:
                break;
        }
    }
    return std::make_tuple(INFINITY, glm::vec3{0}, glm::vec3{0}, 0);
    // means no intersection
}
