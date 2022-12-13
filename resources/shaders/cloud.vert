#version 330 core

layout(location = 0) in vec3 pos_hybrid;

uniform mat4 viewMatrix;
uniform mat4 invViewMatrix;
uniform mat4 projMatrix;
uniform mat4 invProjMatrix;

out vec3 pos_world;

void main() {
    vec4 pos_proj = projMatrix * vec4(pos_hybrid, 1);

    pos_proj.xy = pos_hybrid.xy * pos_proj.w;
//    float xScale = pos_proj.x / pos_proj[3] / pos_hybrid.x;
//    float yScale = pos_proj.y / pos_proj[3] / pos_hybrid.y;
//    vec3 pos_view = vec3(
//                pos_hybrid.x / xScale,
//                pos_hybrid.y / yScale,
//                pos_hybrid.z);
//    pos_world = (invViewMatrix * vec4(pos_view, 1)).xyz;
    vec4 pos_view = invProjMatrix * pos_proj;
    pos_world = (invViewMatrix * pos_view).xyz;

//    pos_proj.xy = 0.5 * pos_hybrid.xy;
//    gl_Position = projMatrix * vec4(pos_view, 1);
    gl_Position = pos_proj;
}
