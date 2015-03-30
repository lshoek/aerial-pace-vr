#include "frameBufferObject.h"


FrameBufferObject::FrameBufferObject()
{
}


FrameBufferObject::~FrameBufferObject()
{
	glDeleteRenderbuffers(1, &rboDepth);
	glDeleteTextures(1, &fboTextureId);
	glDeleteFramebuffers(1, &fboId);
	glDeleteBuffers(1, &vbo_fbo_vertices);
}

void FrameBufferObject::initFBO(){
	//texture
	glGenTextures(1, &fboTextureId);
	glBindTexture(GL_TEXTURE_2D, fboTextureId);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 2048, 2048, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	//depth buffer
	glGenRenderbuffers(1, &rboDepth);
	glBindRenderbuffer(GL_RENDERBUFFER, rboDepth);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT16, 2048, 2048);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);
	/* Framebuffer to link everything together */
	glGenFramebuffers(1, &fboId);
	glBindFramebuffer(GL_FRAMEBUFFER, fboId);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, fboTextureId, 0);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rboDepth);
	GLenum status;
	if ((status = glCheckFramebufferStatus(GL_FRAMEBUFFER)) != GL_FRAMEBUFFER_COMPLETE) {
		fprintf(stderr, "glCheckFramebufferStatus: error %p", status);
		return;
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);	
	return;//oud
	glBindFramebuffer(GL_FRAMEBUFFER, fboId);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 2048, 2048, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, fboTextureId, 0);
	/* init_resources */
	GLfloat fbo_vertices[] = {
		-1, -1,
		1, -1,
		-1, 1,
		1, 1,
	};
	glGenBuffers(1, &vbo_fbo_vertices);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_fbo_vertices);
	glBufferData(GL_ARRAY_BUFFER, sizeof(fbo_vertices), fbo_vertices, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

}