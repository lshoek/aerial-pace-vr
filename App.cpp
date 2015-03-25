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
	//classicFont = new Font("data/aerial-pace-vr/fonts/classicfnt32.fnt", "data/aerial-pace-vr/fonts/classicfnt32.png");
	upArrow.init("UpArrow"); downArrow.init("DownArrow"); leftArrow.init("LeftArrow"); rightArrow.init("RightArrow");
	physics.bullet3Init(wiiMoteWrapper);
	checkers_model = CaveLib::loadModel("data/aerial-pace-vr/models/checkers_sphere.obj", new ModelLoadOptions(1.0f));
	camera = new Camera();

	//caveShader = new ShaderProgram(SHADERLOCATION + "simple.vert", SHADERLOCATION + "simple.frag");
	caveShader = new ShaderProgram(SHADERLOCATION + "threshold.vert", SHADERLOCATION + "threshold.frag");
	caveShader->bindAttributeLocation("a_position", 0);
	caveShader->bindAttributeLocation("a_normal", 1);
	caveShader->bindAttributeLocation("a_texcoord", 2);
	caveShader->link();
	caveShader->use();
	caveShader->setUniformInt("s_texture", 3);

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

	checkers_model->draw(caveShader);
	glUseProgram(0);

	// etc
	//classicFont->drawText("HELLO WORLD!", 10.0f, 10.0f, 0.0f);
	glTranslatef(3, 0, 0);
	DrawWireFrame();
}
