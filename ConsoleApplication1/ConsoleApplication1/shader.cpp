#include "shader.h"
#include <iostream>

shader::shader()
{
}

GLuint shaderCompile(const char* file, int SHADER_TYPE)
{
	std::ifstream t;
	int length;
	t.open(file);      // open input file  
	t.seekg(0, std::ios::end);    // go to the end  
	length = t.tellg();           // report location (this is the length)  
	t.seekg(0, std::ios::beg);    // go back to the beginning  
	char *buffer = new char[length + 1];    // allocate memory for a buffer of appropriate dimension  
	t.read(buffer, length);       // read the whole file into the buffer  
	t.close();                    // close file handle  
	buffer[length] = 0;
	//	

	for (int i = length;buffer[i] != '\n';i--) buffer[i] = 0; //超恶心好吗
	GLuint res = glCreateShader(SHADER_TYPE);
	glShaderSource(res, 1, &buffer, NULL);
	glCompileShader(res);
	int success;
	glGetShaderiv(res, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		char infoLog[512];
		glGetShaderInfoLog(res, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
	}
	else
	{
		std::cout << file << " compiled." << std::endl;
	}
	return res;
}

shader::shader(const char * file1, const char *file2)
{
	vertexShader = shaderCompile(file1, GL_VERTEX_SHADER);
	fragShader = shaderCompile(file2, GL_FRAGMENT_SHADER);

	program = glCreateProgram();
	glAttachShader(program, vertexShader);
	glAttachShader(program, fragShader);
	glLinkProgram(program);
	// Print linking errors if any
	int success;
	glGetProgramiv(program, GL_LINK_STATUS, &success);
	if (!success)
	{
		char infoLog[512];
		glGetProgramInfoLog(program, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
	}
	else
	{
		std::cout << "Program linked" << std::endl;
	}
	glDeleteShader(vertexShader);
	glDeleteShader(fragShader);
}

shader::shader(const char * file1, const char *file2, const char* file3)
{
	vertexShader = shaderCompile(file1, GL_VERTEX_SHADER);
	fragShader = shaderCompile(file2,GL_FRAGMENT_SHADER);
	
	program = glCreateProgram();
	glAttachShader(program, vertexShader);
	
	glAttachShader(program, fragShader);
	if (file3)
	{
		geometryShader = shaderCompile(file3, GL_GEOMETRY_SHADER);
		glAttachShader(program, geometryShader);
	}
	glLinkProgram(program);
	// Print linking errors if any
	int success;
	glGetProgramiv(program, GL_LINK_STATUS, &success);
	if (!success)
	{
		char infoLog[512];
		glGetProgramInfoLog(program, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
	}
	else
	{
		std::cout << "Program linked" << std::endl;
	}
	glDeleteShader(vertexShader);
	glDeleteShader(fragShader);
	if(file3)
		glDeleteShader(geometryShader);
}

void shader::run()
{
	glUseProgram(program);
}

void shader::bind(const char * title, int index)
{
	std::map<std::string, GLuint>::iterator it = entities.find(title);
	GLuint res;
	if (it == entities.end())
	{
		res = glGetUniformLocation(program, title);
		entities.insert(entities.end(), std::make_pair(std::string(title), res));
	}
	else
	{
		res = it->second;
	}
	glUniform1i(res, index);
}

void shader::bind(const char * title, const GLfloat * mat)
{
	std::map<std::string, GLuint>::iterator it = entities.find(title);
	GLuint res;
	if (it == entities.end())
	{
		res = glGetUniformLocation(program, title);
		entities.insert(entities.end(), std::make_pair(std::string(title), res));
	}
	else
	{
		res = it->second;
	}
	glUniformMatrix4fv(res, 1, GL_FALSE, mat);
}

void shader::bind(const char * title, const GLfloat * mat,const int index)
{
	std::string tmp = title;
	std::map<std::string, GLuint>::iterator it = entities.find(title);
	GLuint res;
	if (it == entities.end())
	{
		tmp += "[0]";
		res = glGetUniformLocation(program, tmp.data());
		entities.insert(entities.end(), std::make_pair(std::string(title), res));
	}
	else {
		res = it->second;
	}
	res += index;
	glUniformMatrix4fv(res, 1, GL_FALSE, mat);
}
/*
void shader::bind(const char * title, const GLfloat * mat)
{
	glUniformMatrix4fv(glGetUniformLocation(program, title), 1, GL_FALSE, mat);
}*/

void shader::bind(const char * title, GLfloat x, GLfloat y, GLfloat z)
{
	std::map<std::string, GLuint>::iterator it = entities.find(title);
	GLuint res;
	if (it == entities.end())
	{
		res = glGetUniformLocation(program, title);
		entities.insert(entities.end(), std::make_pair(std::string(title), res));
	}
	else
	{
		res = it->second;
	}
	glUniform3f(res, x, y, z);
}

void shader::bind(const char * title, glm::vec2 vec)
{
	std::map<std::string, GLuint>::iterator it = entities.find(title);
	GLuint res;
	if (it == entities.end())
	{
		res = glGetUniformLocation(program, title);
		entities.insert(entities.end(), std::make_pair(std::string(title), res));
	}
	else
	{
		res = it->second;
	}
	glUniform2f(res, vec.x, vec.y);
}

void shader::bind(const char * title, glm::vec3 vec)
{
	std::map<std::string, GLuint>::iterator it = entities.find(title);
	GLuint res;
	if (it == entities.end())
	{
		res = glGetUniformLocation(program, title);
		entities.insert(entities.end(), std::make_pair(std::string(title), res));
	}
	else
	{
		res = it->second;
	}
	glUniform3f(res,vec.x, vec.y, vec.z);
}

void shader::bindF(const char * title, float f)
{
	std::map<std::string, GLuint>::iterator it = entities.find(title);
	GLuint res;
	if (it == entities.end())
	{
		res = glGetUniformLocation(program, title);
		entities.insert(entities.end(), std::make_pair(std::string(title), res));
	}
	else
	{
		res = it->second;
	}
	glUniform1f(res, f);
}



shader::~shader()
{
	
}
