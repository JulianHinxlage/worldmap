#type vertex
#version 400 core

layout (location=0) in vec3 vPosition;
layout (location=1) in vec3 vNormal;
layout (location=2) in vec2 vTexCoords;

flat out float fMapId;
out vec3 fPosition;

uniform mat4 projection;
uniform mat4 iTransform;


void main(){
    vec4 pos = iTransform * vec4(vPosition, 1.0);
    fPosition = vec3(pos);
    gl_Position = projection * pos;
    fMapId = vTexCoords.x;
}

#type fragment
#version 400 core

flat in float fMapId;
in vec3 fPosition;

struct Teretory{
    vec4 color;
    vec4 id;
};
layout(std140) uniform uTeretories {
    Teretory teretories[1000];
};

uniform sampler2D mapTexture;

out vec4 oColor;
out vec4 oId;

void main(){
    oColor = texture(mapTexture, vec2(fPosition.x / 360 + 0.5, fPosition.z / 180 + 0.5));
    oColor = oColor * 0.0 + teretories[int(fMapId)].color * 1.0;
    
    oId = teretories[int(fMapId)].id;
    oId.a = 1;
}
