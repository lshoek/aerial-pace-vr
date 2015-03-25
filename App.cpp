#pragma comment(lib,"opengl32")
#define _USE_MATH_DEFINES
#include <GL\glew.h>
#include <Windows.h>
#include <CaveLib\model.h>
#include "App.h"
#include <glm/gtc/type_ptr.hpp>

const string App::SHADERLOCATION = "data/aerial-pace-vr/shaders/";

App::App(WiiMoteWrapper* w)
{
	wiiMoteWrapper = w; 
}
App::~App(void){}

void App::init(void)
{
	upArrow.init("UpArrow"); downArrow.init("DownArrow"); leftArrow.init("LeftArrow"); rightArrow.init("RightArrow");
	physics.bullet3Init(wiiMoteWrapper);
	checkers_model = CaveLib::loadModel("data/aerial-pace-vr/models/checkers_sphere.obj", new ModelLoadOptions(10.0f));
	racetrack_model = CaveLib::loadModel("data/aerial-pace-vr/models/racetrack.obj", new ModelLoadOptions(100.0f));
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
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_COLOR_MATERIAL);
	glPushMatrix();
	glTranslatef(0, -1, 0);
	DrawWireFrame();
	glPopMatrix();

	float mvpRaw[16];
	physics.realCar->getWorldTransform().getOpenGLMatrix(mvpRaw);
	glm::mat4 carmvp = glm::make_mat4(mvpRaw);

	glm::mat4 mvp = projectionMatrix * modelViewMatrix;
	glMultMatrixf(glm::value_ptr(carmvp));

	// Update the uniform time variable.
	GLint location_time = -1;
	GLfloat time = 0.0f;
	time = GLfloat(clock()) / GLfloat(CLOCKS_PER_SEC);

	// Simple Shader
	glm::mat4 mvp = projectionMatrix * modelViewMatrix;
	simpleShader->use();
	simpleShader->setUniformFloat("time", time);
	simpleShader->setUniformMatrix4("modelViewProjectionMatrix", mvp);
	checkers_model->draw(simpleShader);

	// Noise Shader
	mvp = glm::rotate(mvp, 45.0f, glm::vec3(0.0f, 1.0f, 0.0f));
	mvp = glm::translate(mvp, glm::vec3(0.0f, -2.0f, 0.0f));
	noiseShader->use();
	noiseShader->setUniformFloat("time", time);
	noiseShader->setUniformMatrix4("modelViewProjectionMatrix", mvp);
	racetrack_model->draw(noiseShader);
	glUseProgram(0);

	// Etc
	DrawWireFrame();
}