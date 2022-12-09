#version 330 core

layout(location = 0) in vec3 pos;
layout(location = 1) in vec3 norm;

out vec3 wpPos;
out vec3 wpNorm;

uniform mat4 ctm;
uniform mat4 n_ctm;
uniform mat4 viewMat;
uniform mat4 projMat;

// main function transforms vertices and normals to world space
// while assign gl_Position to the clip space transformation of the vertex
void main() {
    vec4 interPos = vec4(pos, 1.0);
    interPos = ctm * interPos;
    wpPos = vec3(interPos);

    vec4 interNorm = vec4(norm, 0.0);
    interNorm = n_ctm * interNorm;
    wpNorm = vec3(interNorm);

    gl_Position = projMat * viewMat * ctm * vec4(pos, 1.0);

}
