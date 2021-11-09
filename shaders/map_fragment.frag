#version 450

layout (set = 0, binding = 0) uniform sampler2D shadowMap;

layout (location = 0) in vec2 fragTex;

layout (location = 0) out vec4 color;


float zNear = 0.1; 
float zFar  = 400.0; 
  
float LinearizeDepth(float depth) 
{
    // преобразуем обратно в NDC
    float z = depth * 2.0 - 1.0; 
    return (2.0 * zNear * zFar) / (zFar + zNear - z * (zFar - zNear));  
}


void main() 
{
    float depth = LinearizeDepth(texture(shadowMap, fragTex).r) / zFar;
    color = vec4(vec3(1 - depth), 1.0);
} 
