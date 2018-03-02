#version 420 core
in vec4 FragPos;
in flat int dirlight;

uniform vec3 lightPos;
uniform float far_plane;
out float test;

void main()
{
    /*if(dirlight != 1) {
    // get distance between fragment and light source
    float lightDistance = length(FragPos.xyz - lightPos);
    
    // map to [0;1] range by dividing by far_plane
    lightDistance = lightDistance / far_plane;
    
    // Write this as modified depth
    gl_FragDepth = lightDistance;
    } else {
        test = gl_FragDepth;
    }*/
}  
