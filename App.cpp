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
	//upArrow.init("UpArrow"); downArrow.init("DownArrow"); leftArrow.init("LeftArrow"); rightArrow.init("RightArrow");
	physics.bullet3Init(wiiMoteWrapper);
	cube_model = CaveLib::loadModel("data/aerial-pace-vr/models/cube.obj", new ModelLoadOptions(300.0f));
	checkers_model = CaveLib::loadModel("data/aerial-pace-vr/models/checkers_sphere.obj", new ModelLoadOptions(10.0f));
	sun_model = CaveLib::loadModel("data/aerial-pace-vr/models/checkers_sphere.obj", new ModelLoadOptions(10.0f));
	racetrack_model = CaveLib::loadModel("data/aerial-pace-vr/models/racetrack.obj", new ModelLoadOptions(100.0f));
	
	physics.addFloor(racetrack_model);

	camera = new Camera();
	pointLight.position = glm::vec3(-20.0f, 8.0f, -10.0f);
	pointLight.intensities = glm::vec3(1.0f, 1.0f, 1.0f);

	simpleShader = new ShaderProgram("data/aerial-pace-vr/shaders/simple.vert", "data/aerial-pace-vr/shaders/simple.frag");
	simpleShader->link();
	simpleShader->setUniformInt("s_texture", 0);

	noiseShader = new ShaderProgram("data/aerial-pace-vr/shaders/perlinnoise.vert", "data/aerial-pace-vr/shaders/perlinnoise.frag");
	noiseShader->link();
	noiseShader->setUniformInt("s_texture", 0);

	sunShader = new ShaderProgram("data/aerial-pace-vr/shaders/sunshader.vert", "data/aerial-pace-vr/shaders/sunshader.frag");
	sunShader->link();

	//fbo.endShader = new ShaderProgram("data/aerial-pace-vr/shaders/motionblur.vert", "data/aerial-pace-vr/shaders/motionblur.frag");
	//fbo.endShader->link();
	//fbo.initFBO();

	airnoiseShader = new ShaderProgram("data/aerial-pace-vr/shaders/airnoise.vert", "data/aerial-pace-vr/shaders/airnoise.frag");
	airnoiseShader->link();
	airnoiseShader->setUniformInt("s_texture", 0);

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
	glm::mat4 mvpCube = glm::translate(mvp, glm::vec3(-150.0f, -100.0f, 150.0f));

	// Cube Model (Air)
	airnoiseShader->use();
	airnoiseShader->setUniformFloat("time", time);
	airnoiseShader->setUniformMatrix4("modelViewProjectionMatrix", mvpCube);
	cube_model->draw(airnoiseShader);
	btVector3 carTranslation = physics.realCar->getWorldTransform().getOrigin();
	glm::vec3 glmCarTranslation(carTranslation.x(), -2, carTranslation.z());
	// Checkers Model
	/*simpleShader->use();
	simpleShader->setUniformMatrix4("modelViewMatrix", modelViewMatrix);
	simpleShader->setUniformVec3("light.position", pointLight.position);
	simpleShader->setUniformVec3("light.intensities", pointLight.intensities);
	simpleShader->setUniformFloat("time", time);
	simpleShader->setUniformVec3("materialSpecularColor", glm::vec3(1.0f, 1.0f, 1.0f));
	
	simpleShader->setUniformVec3("cameraPosition", glmCarTranslation);
	simpleShader->setUniformFloat("materialShininess", 0.5f);
	simpleShader->setUniformMatrix4("modelViewProjectionMatrix", mvp);
	checkers_model->draw(simpleShader);*/

	sunShader->use();
	sunShader->setUniformMatrix4("modelViewMatrix", modelViewMatrix);
	sunShader->setUniformFloat("time", time);
	sunShader->setUniformVec3("cameraPosition", glmCarTranslation);
	float scale = 0.01f;
	glm::mat4 sunMat4 = mvp;
	sunMat4 = glm::scale(sunMat4, glm::vec3(scale, scale, scale));
	sunMat4 = glm::translate(sunMat4,pointLight.position);
	sunShader->setUniformMatrix4("modelViewProjectionMatrix", sunMat4);
	sun_model->draw(sunShader);
	
	GLfloat carRotation = 180.0f;
	mvp = glm::rotate(mvp, carRotation, glm::vec3(0.0f, 1.0f, 0.0f));
	mvp = glm::translate(mvp, glmCarTranslation);
	
	// Racetrack
	noiseShader->use();
	noiseShader->setUniformMatrix4("modelViewMatrix", modelViewMatrix);
	noiseShader->setUniformVec3("light.position", pointLight.position);
	noiseShader->setUniformVec3("light.intensities", pointLight.intensities);
	noiseShader->setUniformFloat("time", time);
	noiseShader->setUniformVec3("materialSpecularColor", glm::vec3(1.0f, 1.0f, 1.0f));
	
	noiseShader->setUniformVec3("cameraPosition", glmCarTranslation);
	noiseShader->setUniformFloat("materialShininess", 0.5f);
	noiseShader->setUniformMatrix4("modelViewProjectionMatrix", mvp);
	racetrack_model->draw(noiseShader);	
	//DrawPoint(pointLight.position);

	// Etc
	glUseProgram(0);
	//DrawWireFrame();

	glDisable(GL_DEPTH_TEST);
	//DrawAxii();
	
}

