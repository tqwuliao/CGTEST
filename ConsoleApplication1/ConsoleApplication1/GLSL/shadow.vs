#version 420 core
layout (location = 0) in vec3 position;
layout (location = 3) in ivec4 BoneIDs;
layout (location = 4) in vec4 Weights;

uniform mat4 model;
uniform mat4 transforms[100];
uniform mat4 Tinverse;
uniform bool dynamic;

void main()
{

	mat4 BoneTransform;

	if(dynamic)
	{ 
	BoneTransform = transforms[BoneIDs[0]] * Weights[0] ;
    BoneTransform += transforms[BoneIDs[1]] * Weights[1];
    BoneTransform += transforms[BoneIDs[2]] * Weights[2];
    BoneTransform += transforms[BoneIDs[3]] * Weights[3];
    BoneTransform *= Tinverse;
    BoneTransform = transpose(BoneTransform);
    gl_Position = model * (BoneTransform * vec4(position, 1.0f));
	}
	else
	{
	gl_Position = model * vec4(position, 1.0f);
	}
    
    
}
