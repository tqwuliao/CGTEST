#version 420 core
in vec2 TexCoords;
in vec3 FragPos;
in vec3 Normal;
out vec4 color;

uniform Sampler2D image;
void main()
{
	vec3 _color = vec3(texture(image,TexCoords));
	_color *= Normal;
	color = vec4(_color, 1.0);
}