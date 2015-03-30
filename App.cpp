#pragma comment(lib,"opengl32")
#define _USE_MATH_DEFINES
#include <GL\glew.h>
#include <Windows.h>
#include <CaveLib\model.h>
#include "App.h"
#include <glm/gtc/type_ptr.hpp>
#include <functional>

App::App(WiiMoteWrapper* w)
{
	wiiMoteWrapper = w; 
}
App::~App(void){
	//fbo.~FrameBufferObject();
}

void App::init(void)
{
	upArrow.init("UpArrow"); downArrow.init("DownArrow"); leftArrow.init("LeftArrow"); rightArrow.init("RightArrow");
	physics.bullet3Init(wiiMoteWrapper);
	checkers_model = CaveLib::loadModel("data/aerial-pace-vr/models/checkers_sphere.obj", new ModelLoadOptions(10.0f));
	racetrack_model = CaveLib::loadModel("data/aerial-pace-vr/models/racetrack.obj", new ModelLoadOptions(100.0f));
	
	physics.addFloor(racetrack_model);

	camera = new Camera();

	simpleShader = new ShaderProgram("data/aerial-pace-vr/shaders/simple.vert", "data/aerial-pace-vr/shaders/simple.frag");
	simpleShader->bindAttributeLocation("a_position", 0);
	simpleShader->bindAttributeLocation("a_normal", 1);
	simpleShader->bindAttributeLocation("a_texCoord", 2);
	simpleShader->link();
	simpleShader->setUniformInt("s_texture", 0);

	noiseShader = new ShaderProgram("data/aerial-pace-vr/shaders/simple.vert", "data/aerial-pace-vr/shaders/perlinnoise.frag");
	noiseShader->bindAttributeLocation("a_position", 0);
	noiseShader->bindAttributeLocation("a_normal", 1);
	noiseShader->bindAttributeLocation("a_texCoord", 2);
	noiseShader->link();
	noiseShader->setUniformInt("s_texture", 0);

	sunShader = new ShaderProgram("data/aerial-pace-vr/shaders/sunshader.vert", "data/aerial-pace-vr/shaders/sunshader.frag");
	sunShader->bindAttributeLocation("a_position", 0);
	sunShader->bindAttributeLocation("a_normal", 1);
	sunShader->link();

	//fbo.endShader = new ShaderProgram("data/aerial-pace-vr/shaders/motionblur.vert", "data/aerial-pace-vr/shaders/motionblur.frag");
	//fbo.endShader->link();
	//fbo.initFBO();
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
}

void App::preFrame(double frameTime, double totalTime)
{
	// timer
	clock_t clock_end = clock();
	GLfloat timeFctr = GLfloat(clock_end - clock_start) / CLOCKS_PER_SEC; // calculate time(s) elapsed since last frame
	camera->tf = timeFctr;
	fps = int(1 / timeFctr);
	clock_start = clock();
	physics.updateCar(timeFctr);
	//camera
	/*if (upArrow.getData() == ON)
		camera->updateSpeed();
	if (leftArrow.getData() == ON)
		camera->rotateCamYaw(-1.0f);
	else if (rightArrow.getData() == ON)
		camera->rotateCamYaw(1.0f);*/
}

void App::draw(const glm::mat4 &projectionMatrix, const glm::mat4 &modelViewMatrix)
{
	//bind fbo
	//glBindFramebuffer(GL_FRAMEBUFFER, fbo.fboId);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	//fbo.endShader->use();
	//glBindTexture(GL_TEXTURE_2D, fbo.fboTextureId);
	//glUniform1i(fbo.endShader->getUniformLocation("s_texture"), 0);
	//glEnableVertexAttribArray(1);
	//glBindBuffer(GL_ARRAY_BUFFER, fbo.vbo_fbo_vertices);

	glEnable(GL_TEXTURE_2D);
	glEnable(GL_COLOR_MATERIAL);
	

	float mvpRaw[16];
	physics.realCar->getWorldTransform().getOpenGLMatrix(mvpRaw);
	glm::mat4 carmvp = glm::make_mat4(mvpRaw);
	//glMultMatrixf(glm::value_ptr(carmvp));

	// Update the uniform time variable.
	GLfloat time = GLfloat(clock()) / GLfloat(CLOCKS_PER_SEC);

	// Mvp
	glm::mat4 mvp = projectionMatrix * modelViewMatrix;
	glm::mat3 normalMatrix = glm::mat3(0.5f);
	// Simple Shader
	simpleShader->use();
	simpleShader->setUniformFloat("time", time);
	noiseShader->setUniformVec4("lightPosition", glm::vec4(10.0f, 10.0f, 10.0f, 1.0f));
	noiseShader->setUniformVec3("diffuseReflectivity", glm::vec3(1.0f));
	noiseShader->setUniformVec3("lightSourceIntensity", glm::vec3(1.5f));
	noiseShader->setUniformMatrix4("modelViewMatrix", modelViewMatrix);
	//noiseShader->setUniformMatrix3("normalMatrix", normalMatrix);
	noiseShader->setUniformMatrix4("projectionMatrix", projectionMatrix);
	simpleShader->setUniformMatrix4("modelViewProjectionMatrix", mvp);
	checkers_model->draw(simpleShader);

	// Noise Shader
	drawStage(projectionMatrix * modelViewMatrix, physics.realCar->getWorldTransform().getOrigin(), 180, time);
	DrawWireFrame();


	/*fbo.endShader->use();
	glEnableVertexAttribArray(1);							// en vertex attribute 1
	glEnableVertexAttribArray(2);							// en vertex attribute 2 ook
	glEnableVertexAttribArray(3);							// en vertex attribute 3 ook
	glUniform1i(fbo.endShader->getUniformLocation("s_texture"), 0);
	
	glBindTexture(GL_TEXTURE_2D, fbo.fboTextureId);
	glDisableVertexAttribArray(1);							// disable vertex attribute 1
	glDisableVertexAttribArray(2);							// en vertex attribute 2 ook
	glDisableVertexAttribArray(3);							// en vertex attribute 3 ook
	glVertexAttribPointer(0, 2, GL_FLOAT, false, 2 * 4, &fbo.vbo_fbo_vertices);									//geef aan dat de posities op deze locatie zitten
	glDrawArrays(GL_QUADS, 0, fbo.vbo_fbo_vertices);*/
}

void App::drawStage(const glm::mat4 &mvp1, const btVector3 &translation, float rotation, GLfloat time){
	glm::mat4 mvp = mvp1;	
	//mvp = glm::rotate(mvp, rotation, glm::vec3(0.0f, 1.0f, 0.0f));
	mvp = glm::translate(mvp, glm::vec3(translation.x(), translation.y()*0-2, translation.z()));
	
	noiseShader->use();
	noiseShader->setUniformFloat("time", time);
	noiseShader->setUniformVec4("lightPosition", glm::vec4(10.0f, 10.0f, 10.0f, 1.0f));
	noiseShader->setUniformVec3("diffuseReflectivity", glm::vec3(1.0f));
	noiseShader->setUniformVec3("lightSourceIntensity", glm::vec3(1.5f));
	//noiseShader->setUniformMatrix4("modelViewMatrix", modelViewMatrix);
	//noiseShader->setUniformMatrix3("normalMatrix", normalMatrix);
	//noiseShader->setUniformMatrix4("projectionMatrix", projectionMatrix);
	noiseShader->setUniformMatrix4("modelViewProjectionMatrix", mvp);
	racetrack_model->draw(noiseShader);
}