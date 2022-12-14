#version 330 core

in vec3 wpPos;
in vec3 wpNorm;

// for lighting calculations
uniform vec4 cAmbient;
uniform vec4 cDiffuse;
uniform vec4 cSpecular;
uniform float ka;
uniform float kd;
uniform float ks;
uniform float sh;
uniform vec4 lights[8];
uniform vec4 colors[8];
uniform vec3 att[8];
uniform vec3 spotDir[8];
uniform float thetaO;
uniform float thetaI;
uniform int numLights;
uniform vec4 camPos;

uniform int fogType;
uniform float fogIntensity;

out vec4 fragColor;

float fogScene(vec3 camPos, vec3 wpPos, int fogType){

    //get distance from camera to intersection
    float cameraToPointLen = distance(vec3(camPos), wpPos);

    //calculate fog amount per axis
    float xStart = (camPos[0] - cos(0.6 * camPos[0]) / 0.6) * 0.01;
    float xEnd = (wpPos[0] - cos(0.6 * wpPos[0]) / 0.6) * 0.01;
    float diff = (xEnd - xStart)/(camPos[0] - wpPos[0]);
    float yStart = (camPos[1] - cos(1.2 * camPos[1]) / 1.2) * 0.01;
    float yEnd = (wpPos[1] - cos(1.2 * wpPos[1]) / 1.2) * 0.01;
    diff += (yEnd - yStart)/(camPos[1] - wpPos[1]);
    float zStart = (camPos[2] - cos(0.9 * camPos[2]) / 0.9) * 0.01;
    float zEnd = (wpPos[2] - cos(0.9 * wpPos[2]) / 0.9) * 0.01;
    diff += (zEnd - zStart)/(camPos[2] - wpPos[2]);

    //get total fog amount based on each axis and base value
    float fogTotal = min((cameraToPointLen * (fogIntensity + diff)), 0.7);

    if(fogType == 2){
        float fogTopBound= 0.1;
        float fogLowBound = 0.01;
        if(wpPos[1] > fogTopBound){
            fogTotal = 0;
        }else if(wpPos[1] > fogLowBound){
            float fogFalloff = (fogTopBound - wpPos[1])/(fogTopBound - fogLowBound);
            fogTotal *= fogFalloff;
        }
    }
    return fogTotal;
}

void main() {
    vec3 newNorm = normalize(wpNorm);
    vec4 illumination = vec4(0.0, 0.0, 0.0, 1.0);

     //ambient
    if (wpPos[1] > 0.45) {
        illumination[0] += (ka * 0.95f);
        illumination[1] += (ka * 0.95f);
        illumination[2] += (ka * 0.95f);
    } else{
        illumination[0] += (ka * 0.5f);
        illumination[1] += (ka * 0.4f);
        illumination[2] += (ka * 0.4f);
    }


    for (int i = 0; i < numLights; i++) {
        vec4 light = lights[i];
        vec4 color = colors[i];
        vec3 atten = att[i];
        float inPenum = 1.0;
        vec3 sDir = spotDir[i];
        vec3 L_i;
        float fatt = 1.0;

        if (light[0] == 1) { // matches with enum 0: Directional Light
            L_i = vec3(-light[1], -light[2], -light[3]);
            L_i = normalize(L_i);
        }
        else { // matches with enums 1, 2: Spot, Point Light
            vec3 lightPos = vec3(light[1], light[2], light[3]);
            float d = distance(lightPos, wpPos);
            fatt = min(1.0, (1.0 / (atten[0] + (d * atten[1]) + (atten[2] * d * d))));
            L_i = normalize(lightPos - wpPos);

            if (light[0] == 2) { // only for enum 2: Spot Light
                vec3 lightDir = vec3(sDir[0], sDir[1], sDir[2]);
                lightDir = normalize(lightDir);
                float inter1 = dot(-L_i, lightDir);
                float currX = acos(inter1);
                for (int j = 1; j < 4; j++) { // calculating angular falloff
                    if (thetaI < currX && currX <= thetaO) {
                        float term = (currX - thetaI) / (thetaO - thetaI);
                        float res = -2.0 * (term * term * term) + 3.0 * (term * term);
                        color[j] = (1.0 - res) * color[j];
                    }
                    else if (currX > thetaO) { // if not actually lit by spot light
                        inPenum = 0.0;
                    }
                }
            }
        }

        float diffuse = dot(newNorm, L_i);
        if (diffuse > 0.0) {
            illumination[0] += inPenum * fatt * kd * cDiffuse[0] * diffuse * color[1];
            illumination[1] += inPenum * fatt * kd * cDiffuse[1] * diffuse * color[2];
            illumination[2] += inPenum * fatt * kd * cDiffuse[2] * diffuse * color[3];
        }

        // specular
        vec3 R_i = reflect(-L_i, newNorm);
        R_i = normalize(R_i);

        vec3 dir = vec3(camPos) - wpPos;
        dir = normalize(dir);

        float dot2 = dot(R_i, dir);
        if (dot2 > 0.0) {
            if (sh > 0) {
                dot2 = pow(dot2, sh);
            }
            else { // handling 0-shininess
                dot2 = 1.0;
            }
        }
        else { // handling negative dots
            dot2 = 0.0;
        }
        // specular
        illumination[0] += inPenum * fatt * ks * cSpecular[0] * dot2 * color[1];
        illumination[1] += inPenum * fatt * ks * cSpecular[1] * dot2 * color[2];
        illumination[2] += inPenum * fatt * ks * cSpecular[2] * dot2 * color[3];

    }
    if(fogType != 0){
        float fogTotal = fogScene(vec3(camPos), wpPos, fogType);
        vec4 fogColor = vec4(0.8, 0.8, 0.8, 1 );
        //linerally interpolate between normal color and fog color based on fog intensity
        fragColor = (illumination * (1 - fogTotal) + fogColor * (fogTotal));
    }else{
        fragColor = illumination;
    }

}





