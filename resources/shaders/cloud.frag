#version 330 core

in vec3 pos_world;

uniform float layerDensity;
uniform vec3 noiseSampleScale;
uniform bool adjustColor = false;

uniform float startHeight;
uniform uint heightTexHeight;
uniform sampler1D heightTex;
uniform sampler1D heightGradTex;

uniform sampler3D noiseTex;
uniform sampler3D noiseGradTex;

uniform vec3 cameraPos;
uniform vec3 lightDir; // unused

out vec4 color;

void main() {
    float h = pos_world.y;

    vec3 adjPos = vec3(pos_world.x, h, pos_world.z);

    // Sample cloud density texture
    vec4 noiseSample = texture(noiseTex, adjPos / noiseSampleScale);

    vec3 sampleColor = noiseSample.xyz;
    float density = noiseSample.a;

    /// Cloud geometry

    // Cut off clouds below horizon
    density *= smoothstep(startHeight, startHeight + 2, pos_world.y);
    // Add curve
    float curve = -startHeight + pow(length(pos_world.xz) / 20, 2);
    h += curve;
    // Limit clouds to a reasonable area
    density *= step(0, h) - step(heightTexHeight, h);

    if (density < 0) discard;

    // Sample height density texture
    float heightDensity = texture(heightTex, h / heightTexHeight)[0];
    density *= heightDensity;

    if (density < 0) discard;

    // Give clouds a harder edge
    density = 0.5 * smoothstep(0, 0.1, density);
    // If our volumetric planes are close together, decrease
    // cloud density.
    density = 1 - pow(1 - density, layerDensity);

    // Disabled by default for high computational cost
    if (adjustColor) {
        // To calculate color, we want to use the gradient of
        // the noise and height textures
        vec3 noiseGradSample = texture(noiseGradTex, adjPos / noiseSampleScale).xyz
                / noiseSampleScale;
        float heightGradSample = texture(heightGradTex, h / heightTexHeight)[0]
                / heightTexHeight;
        // Product rule
        vec3 grad = vec3(
                    noiseGradSample.x,
                    noiseSample.a * heightGradSample + noiseGradSample.y * heightDensity,
                    noiseGradSample.z);

        vec3 vecToCamera = cameraPos - pos_world;
        vec3 dirToCamera = normalize(vecToCamera);

        // Adjust color

        /// Adjust light using gradient
        /// Unfortunately, there seems to be an error in the
        /// gradient calculations, so none of the following functions
        /// work very well.

        // sampleColor += 5 * length(cross(dirToCamera, grad));
        // sampleColor += 8 * dot(dirToCamera, grad);
        // sampleColor *= max(0.5, 2 - 3 / sqrt(h + 3));
    }

    // Final color
    color = vec4(sampleColor, density);
}
