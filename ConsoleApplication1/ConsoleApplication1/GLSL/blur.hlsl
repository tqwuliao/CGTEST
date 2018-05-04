#version 420 core
in vec2 TexCoords;
layout (location = 0) out vec4 FragColor;
layout (location = 1) out vec4 NearColor;

uniform sampler2D image;
uniform sampler2D near;

uniform bool horizen;
uniform float weight[6] = float[] (20.0/82, 14.0/82, 9.0/82, 5.0/82, 2.0/82,1.0/82);

void main()
{   
    vec2 tex_offset = 1.0 / textureSize(image, 0); // gets size of single texel
    vec3 result = texture(image, TexCoords).rgb * weight[0]; // current fragment's contribution
    vec3 result1 = texture(near, TexCoords).rgb * weight[0]; // current fragment's contribution
    
    if(horizen)
    for(int i = 1; i <6; ++i)
    {
       vec4 tmpA = texture(near, TexCoords + vec2(tex_offset.x * i, 0.0));
       vec4 tmpB = texture(near, TexCoords - vec2(tex_offset.x * i, 0.0));
       result += texture(image, TexCoords + vec2(tex_offset.x * i, 0.0)).rgb * weight[i];
       result += texture(image, TexCoords - vec2(tex_offset.x * i, 0.0)).rgb * weight[i];
       result1 += tmpA.rgb * weight[i];
       result1 += tmpB.rgb *  weight[i];
    }
    else
    for(int i = 1; i < 6; ++i)
    {
       vec4 tmpA = texture(near, TexCoords + vec2(0.0, tex_offset.y * i));
       vec4 tmpB = texture(near, TexCoords - vec2(0.0, tex_offset.y * i));
       result += texture(image, TexCoords + vec2(0.0, tex_offset.y * i)).rgb * weight[i];
       result += texture(image, TexCoords - vec2(0.0, tex_offset.y * i)).rgb * weight[i];
       result1 += tmpA.rgb * weight[i];
       result1 += tmpB.rgb * weight[i];
    }
    
    FragColor = vec4(result,1.0);//vec4(result, 1.0);
    NearColor = vec4(result1 ,texture(near, TexCoords).a);
}
