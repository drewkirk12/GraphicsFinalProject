#include "params.h"

#include <vector>
#include <glm/glm.hpp>

namespace cloud {

// Volumetric slicing

float sliceDistance = 0.2;

// Perlin noise

std::vector<unsigned int> cloudNoiseResolutions = {
    2, 4, 8, 16
};
int noiseSampleResolution = 32;

// Used by the shader when sampling
glm::vec3 noiseSampleScale = glm::vec3(20, 4, 20);

// Height gradient

float startHeight = -10;
int heightTexHeight = 50;
float heightTexResolution = 1;


float densityScale    = 0.7;

float cloudFloorStart = 8;
float cloudFloorEnd   = 10.5;
float cloudCeilStart  = 12;
float cloudCeilEnd    = 14;

}
