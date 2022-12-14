#pragma once

#include <camera.h>

namespace cloud {
    void initializeClouds();
    void finalizeClouds();

    void setFarPlane(float far);
    void setCamera(const Camera &camera);
    void generateNoise();
    void advanceTime(float time);
    void renderClouds();
}
