#pragma once

#include <glm/glm.hpp>

namespace cloud {

extern float sliceDistance;

// Perlin noise

extern int noiseSampleResolution;

extern float startHeight;
extern int heightTexHeight;
extern float heightTexResolution;

extern glm::vec3 noiseSampleScale;

// Height gradient

extern float densityScale;

extern float cloudFloorStart;
extern float cloudFloorEnd;
extern float cloudCeilStart;
extern float cloudCeilEnd;

}
