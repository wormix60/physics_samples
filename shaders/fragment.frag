#version 450 core
#extension GL_ARB_separate_shader_objects : enable


layout(set = 2, binding = 0) uniform sampler2D tex;
layout(set = 3, binding = 0) uniform sampler2D shadowMap;


layout(set = 4, binding = 0) uniform light{
    vec3 pos;
};


layout(set = 1, binding = 1) uniform Matrices{
    mat4 lightConv;
    mat4 lightProj;
};


layout(location = 0) in vec4 fragPos;
layout(location = 1) in vec2 fragTex;
layout(location = 2) in vec3 fragNormal;

layout(location = 0) out vec4 color;

float shadow(vec4 pos, float ang){
    vec3 proj = pos.xyz / pos.w;
    
    proj.xy = 0.5 * (proj.xy + 1);

    if(proj.x > 1.0 || proj.x < 0.0 || proj.y > 1.0 || proj.y < 0.0 || proj.z > 1.0 || proj.z < 0.0)
        return 0.0;

    float shadow = 0.0;
    vec2 tex = 1.0 / textureSize(shadowMap, 0);
    for (int x = -1; x <= 1; x++) {
        for (int y = -1; y <= 1; y++) {
            
            if (proj.z - max(0.000007 * (1.0 - ang), 0.000002) > texture(shadowMap, proj.xy + vec2(x, y) * tex).r){
                shadow += 1.0;
            }
        }
    }
    shadow /= 9.0;

    return shadow;
}

void main()
{
    vec3 lightColor = vec3(1.0f, 1.0f, 1.0f);

    vec3 col = texture(tex, fragTex).rgb;

    vec3 ambient = 0.2f * col;

    vec3 n = normalize(fragNormal);
    vec3 lightRay = normalize(pos - fragPos.xyz);

    float angle = max(dot(n, lightRay), 0.0);

    vec3 diffuse = angle * lightColor;

    vec4 fragPosLight = lightProj * lightConv * vec4(fragPos.xyz, 1.0f);

    color = vec4(max(ambient, (1.0 - shadow(fragPosLight, angle)) * (diffuse) * col), 1.0);

}