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
	//upArrow.init("UpArrow"); downArrow.init("DownArrow"); leftArrow.init("LeftArrow"); rightArrow.init("RightArrow");
	physics.bullet3Init();
	checkers_model = CaveLib::loadModel("data/aerial-pace-vr/models/checkers_sphere.obj", new ModelLoadOptions(1.0f));
	//classicFont = new Font("data/aerial-pace-vr/fonts/classicfnt32.fnt", "data/aerial-pace-vr/fonts/classicfnt32.png");
	checkers_model = CaveLib::loadModel("data/aerial-pace-vr/models/checkers_sphere.obj", new ModelLoadOptions(1.0f));
	camera = new Camera();
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

	//car
	car.updateCar(timeFctr);
	btVector3 b = physics.realCar->getWorldTransform().getOrigin();
	
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
	checkers_model->draw();
	glPushMatrix();
	camera->refresh();
	
	btVector3 b = physics.realCar->getWorldTransform().getOrigin();
	float angle = physics.realCar->getOrientation().getAngle();
	glTranslatef(b.x(), 0, b.z());
	DrawWireFrame();
	//glRotatef(car.carRadians, 0, 1, 0);	
	for each (btRigidBody* floor in physics.floorParts)
	{
		btVector3 b = floor->getWorldTransform().getOrigin();
		glPushMatrix();
		glTranslatef(b.x(), b.y(), b.z());
		glColor3f(0.3, 0.5, 0.7);
		DrawWireFrame();
		glPopMatrix();
	}
	//classicFont->drawText("HELLO WORLD!", 10.0f, 10.0f, 0.0f);
	
}
