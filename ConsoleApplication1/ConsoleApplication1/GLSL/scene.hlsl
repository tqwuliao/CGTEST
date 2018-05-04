#version 420 core
in vec2 TexCoords;
out vec4 color;
uniform sampler2D screenTexture;
uniform sampler2D bloomBlur;
uniform sampler2D near,far;
uniform mat4 view;
uniform mat4 projection;
void main()
{
     // SSAO part
     float ssao = 0;/*
     if(false) {
       vec4 oripos = texture(far,TexCoords);
       vec3 normal = texture(bloomBlur,TexCoords).rgb;
       
       for(int _angle = 0; _angle < 8; _angle += 1) { 
           float angle = _angle * 1.0 / 3.14;
           vec3 nforward = normalize(vec3(normal.y * sin(angle) + normal.z * cos(angle),-normal.x * sin(angle) - normal.z * cos(angle),-normal.x * cos(angle) + normal.y * cos(angle)));
           for(float angle2 = 0; angle2 <  0.77; angle2 += 0.2 ) {
                vec3 npos = oripos.rgb + (normal * sin(angle2) + nforward * cos(angle2)) * 0.06;
                vec4 npos_t1 = projection * view * vec4(npos,1.0);
                npos_t1.xyz /= npos_t1.w;
                npos_t1 = npos_t1 * 0.5 + vec4(vec3(0.5),0.5);
                float sampleD = texture(far,npos_t1.xy).a;
                float newD = 50.0f / (-(sampleD * 2.0 - 1.0) * 24.0f + 26.0f);
                float newD2 = 50.0f / (-(npos_t1.z * 2.0 - 1.0) * 24.0f + 26.0f);
                float rangeCheck = smoothstep(0.0, 1.0, 0.2 / abs(newD2 - newD));
                if(sampleD + 0.0005 < npos_t1.z) ssao += 0.02 * rangeCheck;
           }
       }
     }*/
     // Ambient
     const float gamma = 1.0;
     float exposure = 2.0;
     vec3 hdrColor=vec3(0,0,0);
     vec4 NEAR = texture(near, TexCoords);
     vec4 FAR = texture(far, TexCoords);
     vec4 FOCUS = texture(screenTexture, TexCoords);
     
     //float distance = FAR.a * (25-1);
     float ratio = pow(clamp( pow((FAR.a - 0.998) * 500.0,4),0.0,1.0),2);
     hdrColor = mix(FOCUS.rgb,NEAR.rgb,(FAR.a<0.995 || FAR.a > 0.9998) ? 1:ratio);
     //hdrColor = FOCUS.rgb;
     //hdrColor = vec3(ratio*ratio);
     vec4 bloom = texture(bloomBlur,TexCoords).rgba;
     hdrColor += bloom.rgb;
     vec3 mapped = vec3(1.0) - exp(-hdrColor * exposure);
     mapped = pow(mapped,vec3(1.0 / gamma));
     //mapped = NEAR.rgb;
     //mapped = FOCUS.rgb+NEAR.rgb+FAR.rgb;//FOCUS.rgb;
     //mapped = vec3(FAR.a*100.0-99.0);
     color = vec4(mapped,1.0);
     //color -= vec4(vec3(ssao),0.0);
}
