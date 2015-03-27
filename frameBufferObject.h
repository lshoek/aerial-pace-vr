#include <GL\glew.h>
#include <CaveLib\CaveLib.h>
#include <CaveLib\Shader.h>
#pragma once
class FrameBufferObject{
public:
	FrameBufferObject();
	~FrameBufferObject();
	ShaderProgram * endShader;
	GLuint fboTextureId;
	GLuint fboId;
	GLuint rboDepth;
	GLuint vbo_fbo_vertices;

	void initFBO();
};