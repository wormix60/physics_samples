#version 450 core
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) out vec4 color;


float zNear = 0.1; 
float zFar  = 400.0; 
  
float LinearizeDepth(float depth) {
    // преобразуем обратно в NDC
    float z = depth * 2.0 - 1.0; 
    return (2.0 * zNear * zFar) / (zFar + zNear - z * (zFar - zNear));  
}


void main() {
    
    // деление на zFar для лучшей наглядности
    //float depth = LinearizeDepth(gl_FragCoord.z) / zFar;
    //color = vec4(vec3(1 - depth), 1.0);

    //color = vec4(vec3(gl_FragCoord.z), 1.0);
}