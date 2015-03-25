#pragma comment(lib,"opengl32")
#define _USE_MATH_DEFINES
#include <GL\glew.h>
#include <Windows.h>
#include <CaveLib\model.h>
#include "App.h"

App::App(WiiMoteWrapper* w)
{
	wiiMoteWrapper = w; 
	car.initCar(w, &physics);
}
App::~App(void){}

void App::init(void)
{
	//classicFont = new Font("data/aerial-pace-vr/fonts/classicfnt32.fnt", "data/aerial-pace-vr/fonts/classicfnt32.png");
	upArrow.init("UpArrow"); downArrow.init("DownArrow"); leftArrow.init("LeftArrow"); rightArrow.init("RightArrow");
	physics.bullet3Init();
	checkers_model = CaveLib::loadModel("data/aerial-pace-vr/models/checkers_sphere.obj", new ModelLoadOptions(1.0f));
	camera = new Camera();

	caveShader = new ShaderProgram("data/aerial-pace-vr/shaders/simple.vert", "data/aerial-pace-vr/shaders/simple.frag");
	caveShader->bindAttributeLocation("a_position", 0);
	caveShader->bindAttributeLocation("a_normal", 1);
	caveShader->bindAttributeLocation("a_texcoord", 2);
	caveShader->link();
	caveShader->use();
	caveShader->setUniformInt("s_texture", 0);

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
	car.updateCar(timeFctr);
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

	// camera
	//camera->refresh();

	// physics
	btVector3 b = physics.realCar->getWorldTransform().getOrigin();
	b = car.direction;
	glTranslatef(b.x(), 0, b.z());
	float angle = physics.realCar->getOrientation().getAngle();
	glRotatef(car.carRadians, 0, 1, 0);

	// shader
	glm::mat4 mvp = projectionMatrix * modelViewMatrix;
	caveShader->use();
	caveShader->setUniformMatrix4("modelViewProjectionMatrix", mvp);
	checkers_model->draw(caveShader);
	glUseProgram(0);

	// etc
	//classicFont->drawText("HELLO WORLD!", 10.0f, 10.0f, 0.0f);
	glScalef(100.0f, 100.0f, 100.0f);
	DrawWireFrame();
}
