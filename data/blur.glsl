#type vertex
#version 400 core

layout (location=0) in vec3 vPosition;
layout (location=1) in vec3 vNormal;
layout (location=2) in vec2 vTexCoords;

out vec3 fPosition;
out vec2 fTexCoords;

uniform mat4 projection;
uniform mat4 iTransform;


void main(){
    vec4 pos = iTransform * vec4(vPosition, 1.0);
    fPosition = vec3(pos);
    gl_Position = projection * pos;
    fTexCoords = vTexCoords;
}

#type fragment
#version 400 core

in vec3 fPosition;
in vec2 fTexCoords;

uniform sampler2D blurTexture;
uniform int steps = 10;
uniform vec2 spread = vec2(0.001, 0);

out vec4 oColor;

void main(){
    
    vec4 color = vec4(0, 0, 0, 0);
    for(int i = -steps; i <= steps; i++){
        color = max(color, texture(blurTexture, clamp(fTexCoords + i * spread, 0, 0.999)));
    }
    //oColor = color / (steps * 2 + 1);
    oColor = color;
}
