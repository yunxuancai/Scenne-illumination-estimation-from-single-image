#version 400 core

layout (location = 0) out vec4 gPosition;
layout (location = 1) out vec4 gAlbedo;
layout (location = 2) out vec4 gNormal;
layout (location = 3) out vec3 gEffects;

in vec3 viewPos;
in vec2 TexCoords;
in vec3 normal;
in vec4 fragPosition;
in vec4 fragPrevPosition;
in vec4 FragPosLightSpace;

const float nearPlane = 1.0f;
const float farPlane = 1000.0f;

uniform vec3 albedoColor;
uniform int shadowMode;
uniform sampler2D texAlbedo;
uniform sampler2D texNormal;
uniform sampler2D texRoughness;
uniform sampler2D texMetalness;
uniform sampler2D texAO;
uniform sampler2D shadowMap;

uniform vec3 lightPos;

float LinearizeDepth(float depth);
vec3 computeTexNormal(vec3 viewNormal, vec3 texNormal);
float ShadowCalculation(vec4 fragPosLightSpace);

void main()
{
    vec3 texNormal = normalize(texture(texNormal, TexCoords).rgb * 2.0f - 1.0f);
    texNormal.g = -texNormal.g;   // In case the normal map was made with DX3D coordinates system in mind

    vec2 fragPosA = (fragPosition.xy / fragPosition.w) * 0.5f + 0.5f;
    vec2 fragPosB = (fragPrevPosition.xy / fragPrevPosition.w) * 0.5f + 0.5f;

    gPosition = vec4(viewPos, LinearizeDepth(gl_FragCoord.z));
    gAlbedo.rgb = vec3(texture(texAlbedo, TexCoords));
//    gAlbedo.rgb = vec3(albedoColor);
    gAlbedo.a =  vec3(texture(texRoughness, TexCoords)).r;
    if(shadowMode==1)
    {
        float shadow = ShadowCalculation(FragPosLightSpace);   
        if(shadow==0)
		    discard;
        
        gAlbedo = vec4(vec3(0), 0.6f);
        gAlbedo.a = 0.02f;
    }
    gNormal.rgb = computeTexNormal(normal, texNormal);
//    gNormal.rgb = normalize(normal);
    gNormal.a = vec3(texture(texMetalness, TexCoords)).r;
    gEffects.r = vec3(texture(texAO, TexCoords)).r;
    gEffects.gb = fragPosA - fragPosB;
}



float LinearizeDepth(float depth)
{
    float z = depth * 2.0f - 1.0f;
    return (2.0f * nearPlane * farPlane) / (farPlane + nearPlane - z * (farPlane - nearPlane));
}


vec3 computeTexNormal(vec3 viewNormal, vec3 texNormal)
{
    vec3 dPosX  = dFdx(viewPos);
    vec3 dPosY  = dFdy(viewPos);
    vec2 dTexX = dFdx(TexCoords);
    vec2 dTexY = dFdy(TexCoords);

    vec3 normal = normalize(viewNormal);
    vec3 tangent = normalize(dPosX * dTexY.t - dPosY * dTexX.t);
    vec3 binormal = -normalize(cross(normal, tangent));
    mat3 TBN = mat3(tangent, binormal, normal);

    return normalize(TBN * texNormal);
}

float ShadowCalculation(vec4 fragPosLightSpace)
{
    // perform perspective divide
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    // transform to [0,1] range
    projCoords = projCoords * 0.5 + 0.5;
    // get closest depth value from light's perspective (using [0,1] range fragPosLight as coords)
    float closestDepth = texture(shadowMap, projCoords.xy).r; 
    // get depth of current fragment from light's perspective
    float currentDepth = projCoords.z;
    // calculate bias (based on depth map resolution and slope)
    vec3 normal = normalize(normal);
    vec3 lightDir = normalize(-lightPos);
    float bias = max(0.05 * (1.0 - dot(normal, lightDir)), 0.005);
    // check whether current frag pos is in shadow
    // float shadow = currentDepth - bias > closestDepth  ? 1.0 : 0.0;
    // PCF
    float shadow = 0.0;
    vec2 texelSize = 1.0 / textureSize(shadowMap, 0);
    for(int x = -1; x <= 1; ++x)
    {
        for(int y = -1; y <= 1; ++y)
        {
            float pcfDepth = texture(shadowMap, projCoords.xy + vec2(x, y) * texelSize).r; 
            shadow += currentDepth - bias > pcfDepth  ? 1.0 : 0.0;        
        }    
    }
    shadow /= 9.0;
    
    // keep the shadow at 0.0 when outside the far_plane region of the light's frustum.
    if(projCoords.z > 1.0)
        shadow = 0.0;
        
    return shadow;
}