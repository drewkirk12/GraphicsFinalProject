#version 330 core

in vec3 pos_world;

uniform float layerDensity = 1.0;

uniform sampler3D noiseTex;
uniform sampler3D gradTex;

uniform vec3 cameraPos;
uniform vec3 lightDir; // unused

out vec4 color;

void main() {
    float h = pos_world.y;
    int sampleDist = 15;

    vec3 adjPos = vec3(pos_world.x, h, pos_world.z);

    // Sample cloud density texture
    vec4 noiseSample = texture(noiseTex, adjPos / sampleDist);

//    if (noiseSample.a > 0) {
//        noiseSample.a *= texture(noiseTex, adjPos / 4).a;
//        gradSample -= 0.5 * texture(gradTex, adjPos / 4).xyz / 5;
//    }

    vec3 sampleColor = noiseSample.xyz;
    float density = noiseSample.a;

    /// Cloud geometry

    // Only allow clouds at certain heights
    float curve = 10 - pow(length(pos_world.xz) / 10, 2);
    h -= curve;
    // Only allow clouds at some heights
    density *= smoothstep(0, 1, h)
            - smoothstep(10, 20, h);
    // Decrease density at high altitudes
    density *= min(1, 3 / sqrt(h));
    // Cut off clouds at horizon
    density *= smoothstep(0, 2, pos_world.y);

    if (density < 0) discard;

    vec3 gradSample = texture(gradTex, adjPos / sampleDist).xyz;
    vec3 vecToCamera = cameraPos - pos_world;
    vec3 dirToCamera = normalize(vecToCamera);

    /// Consider adding color based on the gradient
    // sampleColor *= 0.5 + gradSample;
    /// Adjust light using gradient
    /// Doesn't look great, but the clouds look a little more interesting
    sampleColor += 2 * dot(dirToCamera, gradSample);

    // Give clouds a harder edge
    density = 0.5 * smoothstep(0, 0.1, density);

    // If our volumetric planes are close together, decrease
    // cloud density.
    density = 1 - pow(1 - density, layerDensity);

    // Final color
    color = vec4(sampleColor, density);
}
