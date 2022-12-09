#version 330 core

// Task 16: Create a txt coordinate in variable
in vec2 UV;

uniform sampler2D txt;
uniform bool invert;
uniform bool kernel;
uniform float im_height;
uniform float im_width;

out vec4 fragColor;

// fragment shader that applies inverted color filter or
// sharpening depending on user settings.

void main()
{
    fragColor = texture(txt, UV);
    float tx = UV[0];
    float ty = UV[1];
    float x_inc = 2.0 / im_width;
    float y_inc = 2.0 / im_height;
    if (invert) {
        fragColor[0] = 1 - fragColor[0];
        fragColor[1] = 1 - fragColor[1];
        fragColor[2] = 1 - fragColor[2];
    }
    if (kernel) { // matchng the kernel given in handout for sharpening
        vec2 tlv = vec2((tx - x_inc), (ty + y_inc));
        vec2 tmv = vec2(tx, (ty + y_inc));
        vec2 trv = vec2((tx + x_inc), (ty + y_inc));
        vec2 clv = vec2((tx - x_inc), ty);
        vec2 cmv = vec2(tx, ty);
        vec2 crv = vec2((tx + x_inc), ty);
        vec2 blv = vec2((tx - x_inc), (ty - y_inc));
        vec2 bmv = vec2(tx, (ty - y_inc));
        vec2 brv = vec2((tx + x_inc), (ty - y_inc));

        vec4 ftl = -1.0 * texture(txt, tlv);
        vec4 ftm = -1.0 * texture(txt, tmv);
        vec4 ftr = -1.0 * texture(txt, trv);
        vec4 fcl =  -1.0 * texture(txt, clv);
        vec4 fcm = 17.0 * texture(txt, cmv);
        vec4 fcr = -1.0 * texture(txt, crv);
        vec4 fbl = -1.0 * texture(txt, blv);
        vec4 fbm = -1.0 * texture(txt, bmv);
        vec4 fbr = -1.0 * texture(txt, brv);
        vec4 total = (1.0 / 9.0) * (ftl + ftm + ftr + fcl + fcm + fcr + fbl + fbm + fbr);
        fragColor = total;
    }
}
