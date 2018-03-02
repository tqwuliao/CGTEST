#version 420 core
layout (triangles) in;
layout (triangle_strip, max_vertices=18) out;

uniform mat4 shadowMatrices[7];
uniform int DirLight;

out vec4 FragPos; // FragPos from GS (output per emitvertex)
out flat int dirlight;

void main()
{
    /*if(DirLight == 0) {
    dirlight = 0;
    for(int face = 0; face < 6; ++face)
    {
        gl_Layer = face; // built-in variable that specifies to which face we render.
        for(int i = 0; i < 3; ++i) // for each triangle's vertices
        {
            FragPos = gl_in[i].gl_Position;
            gl_Position = shadowMatrices[face] * FragPos;
            EmitVertex();
        }    
        EndPrimitive();
    }
    }*/
    dirlight = 1;
    gl_Layer = 6;
        for(int i = 0; i < 3; ++i) // for each triangle's vertices
        {
            FragPos = gl_in[i].gl_Position;
            gl_Position = shadowMatrices[6] * FragPos;
            EmitVertex();
        }    
        EndPrimitive();
}  
