#pragma once

#include <GL/glew.h>

namespace cloud {
void generateNoise(unsigned int sampleResolution,
                   GLuint noiseTex, GLuint gradTex);
}
