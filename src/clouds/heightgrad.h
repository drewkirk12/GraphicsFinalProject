#pragma once

#include <GL/glew.h>
#include <GL/gl.h>

namespace cloud {
void generateHeightGradient(int height, float resolution,
        GLuint densityTex, GLuint gradTex);
}
