#include "TextRenderer.h"

extern int Scwidth, Scheight;
extern glm::mat4 Transform_2D;

glm::vec3 TextRenderer::textColors[10] = { glm::vec3(0),
glm::vec3(0), 
glm::vec3(0), 
glm::vec3(0), 
glm::vec3(0), 
glm::vec3(0), 
glm::vec3(0), 
glm::vec3(0), 
glm::vec3(0), 
glm::vec3(0)};


TextRenderer & TextRenderer::Instance()
{
	static TextRenderer res;
	
	return res;
	// TODO: 在此处插入 return 语句
}

TextRenderer::TextRenderer() : textShader("./GLSL/text.v.txt","./GLSL/text.f.txt")
{
	
	TextRenderer::textColors[1] = glm::vec3{ 0.8f,0,0 };
	FT_Library ft_lib;
	if (FT_Init_FreeType(&ft_lib))
	{
		std::cout << "Freetype init error.\n";
	}
	else std::cout << "Freetype init success.\n";

	FT_Face ft_face;
	if (FT_New_Face(ft_lib, "resources/fonts/Cuprum-Italic.ttf", 0, &ft_face))
	{
		std::cout << "Load Font resource fail.\n";
	}
	// width set to zero = automatically.
	FT_Set_Pixel_Sizes(ft_face, 0, 48);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	GLuint texture[127];
	glGenTextures(127, texture);
	for (int c = 32;c < 127;c++)
	{
		if (FT_Load_Char(ft_face, c, FT_LOAD_RENDER))
		{
			std::cout << "error load character: " << (char)c << "\n";
			continue;
		}
		glBindTexture(GL_TEXTURE_2D, texture[c]);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RED,
			ft_face->glyph->bitmap.width,
			ft_face->glyph->bitmap.rows,
			0, GL_RED, GL_UNSIGNED_BYTE,
			ft_face->glyph->bitmap.buffer);

		// texture options
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glGenerateMipmap(GL_TEXTURE_2D);

		C_DEF cdef;
		cdef.id = texture[c];
		cdef.size = { ft_face->glyph->bitmap.width,ft_face->glyph->bitmap.rows };
		cdef.margin = { ft_face->glyph->bitmap_left,ft_face->glyph->bitmap_top };
		cdef.offset = ft_face->glyph->advance.x;
		CDEFS.insert(std::make_pair(c,cdef));
	}
	FT_Done_Face(ft_face);
	FT_Done_FreeType(ft_lib);

	glBindTexture(GL_TEXTURE_2D, 0);

	glCreateVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 6 * 5, NULL, GL_DYNAMIC_DRAW); 
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), 0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

TextRenderer::~TextRenderer()
{
	glDeleteBuffers(1, &VBO);
	glDeleteVertexArrays(1, &VAO);
}

void TextRenderer::draw(std::string str, glm::vec3 color, glm::mat4 projection)
{
	textShader.run();
	projection = Transform_2D * projection;
	textShader.bind("project",glm::value_ptr(projection));
	textShader.bind("text_color", color.x,color.y,color.z);
	textShader.bind("texture_sampler", 1);
	float offset_x = 0;
	float offset_y = 0;
	glBindVertexArray(VAO);
	glActiveTexture(GL_TEXTURE1);
	char c;
	TextRenderer::textColors[0] = color;
	for (int i = 0;i < str.size();i++)
	{
		c = str[i];
		if (c == '\\') { // 转义字符
			if (str[i + 1] == 'c') { // change color
				std::string tmp;
				tmp += str[i + 3];
				glm::vec3 _color = TextRenderer::textColors[atoi(tmp.data())];
				textShader.bind("text_color", _color.x, _color.y, _color.z);
				i += 4;
				continue;
			}
		}
		C_DEF cd = CDEFS[c];
		float x_pos = offset_x + cd.margin.x;
		float y_pos = offset_y - cd.size.y + cd.margin.y;

		float w = cd.size.x;
		float h = cd.size.y;

		float vertices[6][5] = 
		{
			{x_pos,  y_pos + h,0,0,0},
			{x_pos,  y_pos,    0,0,1},
			{x_pos+w,y_pos,    0,1,1},
			{x_pos,  y_pos + h,0,0,0},
			{x_pos+w,y_pos,    0,1,1},
			{x_pos+w,y_pos + h,0,1,0}
		};
		
		glBindTexture(GL_TEXTURE_2D, cd.id);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
		//glBindBuffer(GL_ARRAY_BUFFER, 0);
		
		glDrawArrays(GL_TRIANGLES, 0, 6);
		offset_x += (cd.offset >> 6);
	}

	glBindVertexArray(0);
	glBindTexture(GL_TEXTURE_2D,0);
}
