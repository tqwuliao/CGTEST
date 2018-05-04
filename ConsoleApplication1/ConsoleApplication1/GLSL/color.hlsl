#version 420 core

struct Material {
sampler2D texture_diffuse1;
sampler2D texture_specular1;
sampler2D texture_diffuse2;
sampler2D texture_specular2;
sampler2D normalMap1;
sampler2D opacityMap1;
bool normalhas;
float shininess;
bool opacity;
};

// defination for Directional Light
struct DirLight {
vec3 direction;
vec3 ambient;
float far;
vec3 diffuse;
vec3 specular;
mat4 transform;
};

// defination for Point Light
struct PointLight {    
    vec3 position;
    
    float constant;
    float linear;
    float quadratic;  
    int enable;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};  

#define NR_POINT_LIGHTS 1

in vec3 Normal;
in vec3 FragPos;
in vec2 TexCoords;
in vec2 scPos;
in vec4 test;
in float hit;
layout (location = 0) out vec4 FragColor;
layout (location = 1) out vec4 BrightColor;
layout (location = 2) out vec4 Far;

uniform Material material;
uniform DirLight dirLight;

uniform float far_plane;

uniform PointLight pointLight[NR_POINT_LIGHTS];
uniform vec3 viewPos;
uniform mat4 transforms[100];
uniform samplerCube shadowMap;
uniform sampler2D shadowMap2;
uniform int showHit=0;
//in vec4 FragPosLightSpace;



vec3 sampleOffsetDirections[20] = vec3[]
(
   vec3( 1,  1,  1), vec3( 1, -1,  1), vec3(-1, -1,  1), vec3(-1,  1,  1), 
   vec3( 1,  1, -1), vec3( 1, -1, -1), vec3(-1, -1, -1), vec3(-1,  1, -1),
   vec3( 1,  1,  0), vec3( 1, -1,  0), vec3(-1, -1,  0), vec3(-1,  1,  0),
   vec3( 1,  0,  1), vec3(-1,  0,  1), vec3( 1,  0, -1), vec3(-1,  0, -1),
   vec3( 0,  1,  1), vec3( 0, -1,  1), vec3( 0, -1, -1), vec3( 0,  1, -1)
);   

vec2 sampleOffsetDirections2[9] = vec2[]
(
   vec2( 1,  1), vec2( 1, -1), vec2(-1, -1), vec2(-1,  1), 
   vec2( 1,  0), vec2(-1,  0), vec2( 0,  1), vec2( 0, -1), vec2( 0, 0)
);   

float ShadowCalculation(PointLight light,vec3 fragPos)
{
    // Get vector between fragment position and light position
    vec3 fragToLight = fragPos - light.position;
    // Now get current linear depth as the length between the fragment and light position
    float currentDepth = length(fragToLight);
    // Now test for shadows
    float shadow = 0.0;
float bias = 0.1;
int samples = 20;
float viewDistance = length(viewPos - fragPos);
float diskRadius = 0.0005;
for(int i = 0; i < samples; ++i)
{
    vec3 tmp = fragToLight + sampleOffsetDirections[i] * diskRadius;
    
    float closestDepth = texture(shadowMap, tmp).r;
    closestDepth *= far_plane;   // Undo mapping [0;1]
    if(length(tmp) < far_plane && currentDepth - bias > closestDepth)
        shadow += 1.0;
}

shadow /= float(samples); 
return shadow; 
}  

float Shadow2Calculation(DirLight light,vec3 fragPos)
{
    // Get vector between fragment position and light position
    vec4 fragToLight = light.transform * vec4(fragPos,1.0);
     if(fragToLight.w != 0) fragToLight /= fragToLight.w;
    fragToLight = fragToLight * 0.5 + vec4(0.5,0.5,0.5,0.5);
    // Now get current linear depth as the length between the fragment and light position
    float currentDepth = fragToLight.z * light.far;
    // Now test for shadows
    float shadow = 0.0;
float bias = 0.1;
int samples = 9;
float viewDistance = length(viewPos - fragPos);
float diskRadius = 0.0002;
for(int i = 0; i < 12; ++i)
{
    vec2 tmp =  fragToLight.xy + sampleOffsetDirections2[i > 8 ? 8 : i] * diskRadius;
    float closestDepth = texture(shadowMap2,tmp).r;
    closestDepth *= light.far;   // Undo mapping [0;1]
    if( currentDepth - bias > closestDepth)
        shadow += 1.0;
}

shadow /= float(12); 
return shadow; 
}  

vec3 CalcDirLight(DirLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
    vec3 lightDir = normalize(-light.direction);
    float shadow2 = Shadow2Calculation(light,fragPos);
    // Diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // Specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    vec3 halfwayDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(normal, halfwayDir), 0.0), material.shininess);
    // Combine results
    vec3 ambient  = light.ambient  * vec3(texture(material.texture_diffuse1, TexCoords));
    vec3 diffuse  = light.diffuse  * diff * vec3(texture(material.texture_diffuse1, TexCoords));
    vec3 specular = light.specular * spec * vec3(texture(material.texture_specular1, TexCoords));
    return (ambient + (diffuse + specular)*(1.0-shadow2));
}  

// Calculates the color when using a point light.
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
    if(light.enable != 1) return vec3(0,0,0);
    vec3 lightDir = normalize(light.position - fragPos);
    float shadow = ShadowCalculation(light,fragPos);
    // Diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // Specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    vec3 halfwayDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(normal,halfwayDir), 0.0), material.shininess);
    // Attenuation
    float distance    = length(light.position - fragPos);
    float attenuation = 1.0f / (light.constant + light.linear * distance + 
  			     light.quadratic * (distance * distance));    
    // Combine results
    vec3 ambient  = light.ambient  * vec3(texture(material.texture_diffuse1, TexCoords));
    vec3 diffuse  = light.diffuse  * diff * vec3(texture(material.texture_diffuse1, TexCoords));
    vec3 specular = light.specular * spec * vec3(texture(material.texture_specular1, TexCoords));
    ambient  *= attenuation;
    diffuse  *= attenuation;
    specular *= attenuation;
    return (ambient + (diffuse + specular)*(1.0-shadow));
} 

const float offset = 1.0 / 300;

void main()
{
    // Ambient
    vec3 normal = texture(material.normalMap1, TexCoords).gba;
    
    // Transform normal vector to range [-1,1]
    normal = normalize(normal);   
    vec3 norm = normalize(Normal);
    if(material.normalhas) norm = normalize(normal+norm);
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 result=vec3(0,0,0);
    result += CalcDirLight(dirLight,norm,FragPos,viewDir);
    FragColor=texture(material.texture_diffuse1, TexCoords);
    for(int i = 0; i < NR_POINT_LIGHTS; i++)
  	     result += CalcPointLight(pointLight[i],norm,FragPos,viewDir);
    FragColor = vec4(result,1);
    float z = gl_FragCoord.z * 2.0 - 1.0; // Back to NDC 
    z = (2.0 * 1.0 * far_plane) / (far_plane + 1.0 - z * (far_plane - 1.0)) ;	
    Far = vec4(FragPos,gl_FragCoord.z);
    
    if(material.opacity) 
    { 
        FragColor.a = texture(material.opacityMap1, TexCoords).a;
    }
    if(z > far_plane - 0.0625) FragColor.a = mix(FragColor.a,0,(0.0625+z-far_plane)*16);
    float brightness = dot(FragColor.rgb, vec3(0.2126, 0.7152, 0.0722));
    if(brightness > 0.85) //BrightColor = vec4(0.5);
        BrightColor = vec4(FragColor.rgb, 1.0); else BrightColor = vec4(0.0);
    //FragColor = vec4(vec3(texture(shadowMap2,scPos/2.0+vec2(0.5)).r),1.0);
    //FragColor = vec4(dirLight.diffuse,1.0);
}
