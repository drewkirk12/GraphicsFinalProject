#include "params.h"

#include <glm/glm.hpp>

namespace cloud {

// Volumetric slicing

float sliceDistance = 0.1;

// Perlin noise

int noiseSampleResolution = 64;

float startHeight = -10;
int heightTexHeight = 50;
float heightTexResolution = 1;

glm::vec3 noiseSampleScale = glm::vec3(10, 2, 10);

// Height gradient

float densityScale    = 2;

float cloudFloorStart = 2;
float cloudFloorEnd   = 6.5;
float cloudCeilStart  = 10;
float cloudCeilEnd    = 12;

}
