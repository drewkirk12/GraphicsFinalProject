#ifndef SHAPEFUNCTIONS_H
#define SHAPEFUNCTIONS_H


/*
 * ============= Very often used functions for shape primitive classes go here
 */
#include <glm/glm.hpp>
#include <vector>
class shapeFunc
{
public:
    shapeFunc();
    // used to insert just (1) vector into the data
    static void insertVec3(std::vector<float> &data, glm::vec3 v) {
        data.push_back(v.x);
        data.push_back(v.y);
        data.push_back(v.z);
    }
    /**
     * @brief insertTriangle - inserts a "triangle" into
     *  the data from 3 vertices and 3 normals
     */
    static void insertTriangle(std::vector<float> &data, glm::vec3 v1, glm::vec3 n1,
                           glm::vec3 v2, glm::vec3 n2, glm::vec3 v3, glm::vec3 n3) {
        data.push_back(v1.x); data.push_back(v1.y); data.push_back(v1.z);
        data.push_back(n1.x); data.push_back(n1.y); data.push_back(n1.z);
        data.push_back(v2.x); data.push_back(v2.y); data.push_back(v2.z);
        data.push_back(n2.x); data.push_back(n2.y); data.push_back(n2.z);
        data.push_back(v3.x); data.push_back(v3.y); data.push_back(v3.z);
        data.push_back(n3.x); data.push_back(n3.y); data.push_back(n3.z);
    }
};

#endif // SHAPEFUNCTIONS_H
