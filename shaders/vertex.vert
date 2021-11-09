#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(set = 0, binding = 0) uniform Pos{
    mat4 pos;
    mat4 rot;
};


layout(set = 1, binding = 0) uniform Matrices{
    mat4 conv; 
    mat4 proj;
};


layout(location = 0) in vec3 vert;
layout(location = 1) in vec2 vertTex;
layout(location = 2) in vec3 vertNormal;


layout(location = 0) out vec4 fragPos;
layout(location = 1) out vec2 fragTex;
layout(location = 2) out vec3 fragNormal;


void main(void)
{
    gl_Position = proj * conv * pos * rot * vec4(vert, 1.0);

    vec4 worldPos = pos * rot * vec4(vert, 1.0);
    fragPos = worldPos;
    fragNormal = (rot * vec4(vertNormal, 1.0)).xyz;
    fragTex = vertTex;
}
