#include "renderer.hpp"

#include <glad/glad.h>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include <iostream>
#include <QuickMath/quickmath.h>

//--------------------------------------------------------------------------------------------------------------------------------//

static bool _renderer_load_into_buffer(const char* path, char** buffer);
static void _update_projection_mat();

//--------------------------------------------------------------------------------------------------------------------------------//

static unsigned int g_spriteProgram;
static unsigned int g_spriteVAO;

static int g_windowW, g_windowH;
static int g_logicalW, g_logicalH;
static DNmat4 g_projectionMat;

//--------------------------------------------------------------------------------------------------------------------------------//

bool renderer_init(int windowW, int windowH, int logicalW, int logicalH)
{
	//load vertex shader:
	//---------------------------------
	char* vertexSource = 0;
	if(!_renderer_load_into_buffer("shaders/sprite.vert", &vertexSource))
		return false;

	unsigned int vertex;
	int vertexSuccess;

	vertex = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertex, 1, (const GLchar * const*)&vertexSource, NULL);
	glCompileShader(vertex);
	glGetShaderiv(vertex, GL_COMPILE_STATUS, &vertexSuccess);
	if(!vertexSuccess)
	{
		GLsizei logLength;
		char message[1024];
		glGetShaderInfoLog(vertex, 1024, &logLength, message);
		std::cout << "FAILED TO COMPILE VERTEX SHADER WITH THE FOLLOWING INFO LOG:\n" << message << std::endl;
		return false;
	}

	//load fragment shader:
	//---------------------------------
	char* fragmentSource = 0;
	if(!_renderer_load_into_buffer("shaders/sprite.frag", &fragmentSource))
		return false;

	unsigned int fragment;
	int fragmentSuccess;

	fragment = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragment, 1, (const GLchar * const*)&fragmentSource, NULL);
	glCompileShader(fragment);
	glGetShaderiv(fragment, GL_COMPILE_STATUS, &fragmentSuccess);
	if(!fragmentSuccess)
	{
		GLsizei logLength;
		char message[1024];
		glGetShaderInfoLog(fragment, 1024, &logLength, message);
		std::cout << "FAILED TO COMPILE FRAGMENT SHADER WITH THE FOLLOWING INFO LOG:\n" << message << std::endl;
		return false;
	}

	//link program:
	//---------------------------------
	g_spriteProgram = glCreateProgram();
	glAttachShader(g_spriteProgram, vertex);
	glAttachShader(g_spriteProgram, fragment);
	glLinkProgram(g_spriteProgram);

	int programSuccess;
	glGetProgramiv(g_spriteProgram, GL_LINK_STATUS, &programSuccess);
	if(!programSuccess)
	{
		std::cout << "FAILED TO LINK SPRITE PROGRAM\n";
		return false;
	}

	//free shader stuff:
	//---------------------------------
	free(vertexSource);
	free(fragmentSource);
	glDeleteShader(vertex);
	glDeleteShader(fragment);

	//generate sprite vao:
	//---------------------------------
	GLfloat vertices[] = 
	{
		 1.0f,  1.0f, 1.0f, 1.0f,
		 1.0f, -1.0f, 1.0f, 0.0f,
		-1.0f, -1.0f, 0.0f, 0.0f,
		-1.0f,  1.0f, 0.0f, 1.0f
	};
	GLuint indices[] = 
	{
		0, 1, 3,
		1, 2, 3
	};

	GLuint VBO, EBO;
	glGenVertexArrays(1, &g_spriteVAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	glBindVertexArray(g_spriteVAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (void*)(long long)0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (void*)(long long)(2 * sizeof(GL_FLOAT)));
	glEnableVertexAttribArray(1);

	//update projection matrix:
	//---------------------------------
	g_windowW = windowW;
	g_windowH = windowH;
	g_logicalW = logicalW;
	g_logicalH = logicalH;
	_update_projection_mat();

	return true;	
}

void renderer_quit()
{
	glDeleteVertexArrays(1, &g_spriteVAO);
	glDeleteProgram(g_spriteProgram);
}

void renderer_set_window_size(int w, int h)
{
	g_windowW = w;
	g_windowH = h;
	_update_projection_mat();
}

void renderer_set_logical_size(int w, int h)
{
	g_windowW = w;
	g_windowH = h;
	_update_projection_mat();
}

//--------------------------------------------------------------------------------------------------------------------------------//

Texture renderer_load_texture(const char* path)
{
	Texture result;

	glGenTextures(1, (GLuint*)&result);
	glBindTexture(GL_TEXTURE_2D, result);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	int w, h, numChannels;
	unsigned char* raw = stbi_load(path, &w, &h, &numChannels, 0);
	if(!raw)
	{
		std::cout << "FAILED TO LOAD TEXTURE \"" << path << "\"\n";
		return -1;
	}

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, raw);
	stbi_image_free(raw);

	return result;
}

void renderer_free_texture(Texture tex)
{
	glDeleteTextures(1, (GLuint*)&tex);
}

void renderer_draw_texture(Texture tex, int x, int y, int w, int h, float angle)
{
	DNmat4 model = DN_mat4_translate({(float)x, (float)y, 0.0f});
	model = DN_mat4_mult(model, DN_mat4_rotate_euler({0.0f, 0.0f, angle}));
	model = DN_mat4_mult(model, DN_mat4_scale({(float)w * 0.5f, (float)h * 0.5f, 0.0f}));

	glUseProgram(g_spriteProgram);
	glUniformMatrix4fv(glGetUniformLocation(g_spriteProgram, "model"), 1, GL_FALSE, (GLfloat*)&model);
	glUniform1i(glGetUniformLocation(g_spriteProgram, "tex"), 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, tex);

	glBindVertexArray(g_spriteVAO);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}

//--------------------------------------------------------------------------------------------------------------------------------//

static bool _renderer_load_into_buffer(const char* path, char** buffer)
{
	*buffer = 0;
	long length;
	FILE* file = fopen(path, "rb");

	if(file)
	{
		bool result = false;

		fseek(file, 0, SEEK_END);
		length = ftell(file);
		fseek(file, 0, SEEK_SET);
		*buffer = (char*)malloc(length + 1);

		if(*buffer)
		{
			fread(*buffer, length, 1, file);
			(*buffer)[length] = '\0';
			result = true;
		}
		else
		{
			std::cout << "FAILED TO ALLOCATE MEMORY FOR SHADER SOURCE CODE\n";
			result = false;
		}

		fclose(file);
		return result;
	}
	else
	{
		std::cout << "FAILED TO OPEN FILE \"" << path << "\" FOR READING\n";
		return false;
	}
}

static void _update_projection_mat()
{
	int left, right, bot, top;

	float windowAspect = (float)g_windowW / g_windowH;
	float logicalAspect = (float)g_logicalW / g_logicalH;
	if(windowAspect > logicalAspect)
	{
		top = 0;
		bot = g_logicalH;

		float scale = (float)g_logicalH / g_windowH;

		left = (int)((g_logicalW - g_windowW * scale) * 0.5f);
		right = g_logicalW - left;
	}
	else
	{
		left = 0;
		right = g_logicalW;

		float scale = (float)g_logicalW / g_windowW;

		top = (int)((g_logicalH - g_windowH * scale) * 0.5f);
		bot = g_logicalH - top;
	}

	g_projectionMat = DN_mat4_orthographic(left, right, bot, top, 0.0f, 100.0f);

	glUseProgram(g_spriteProgram);
	glUniformMatrix4fv(glGetUniformLocation(g_spriteProgram, "projection"), 1, GL_FALSE, (GLfloat*)&g_projectionMat);
}