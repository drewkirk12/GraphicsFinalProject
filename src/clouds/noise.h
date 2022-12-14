#pragma once

#include <vector>
#include <GL/glew.h>

namespace cloud {
void generateNoise(
        const std::vector<unsigned int> &noiseResolutions,
        unsigned int sampleResolution,
        GLuint noiseTex, GLuint gradTex);
}
