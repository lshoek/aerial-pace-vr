#pragma comment(lib,"opengl32")
#include <GL\glew.h>
#include <Windows.h>
#include <iostream>
#include "App.h"
#include <CaveLib\CaveLib.h>
#include <CaveLib\texture.h>
#include <CaveLib\model.h>

App::App(WiiMoteWrapper * w){
	wiiMoteWrapper = w; 
}
App::~App(void){
	delete world;
	delete solver;
	delete collisionConfiguration;
	delete dispatcher;
	delete broadphase;
}

int App::updateCarSpeed(GLfloat timeFactor){
	
	if (wiiMoteWrapper->buttonHome && (wiiMoteWrapper->buttonOne && wiiMoteWrapper->buttonTwo))//pauze, zolang home is ingedrukt
		return -1;
	GLfloat deltaForce = 0;//extra kracht
	if (wiiMoteWrapper->buttonOne)//speed goes down
	{
		deltaForce -= car.MAXFORCE * timeFactor;
	}
	if (wiiMoteWrapper->buttonTwo)//speed goes up
	{
		deltaForce += car.MAXFORCE * timeFactor;
	}
	car.steeringWheelDegrees = wiiMoteWrapper->degrees;
	car.motorForce += deltaForce;
	if (car.motorForce < -car.MAXFORCE)
		car.motorForce = -car.MAXFORCE;
	if (car.motorForce > car.MAXFORCE)
		car.motorForce = car.MAXFORCE;
	//zet de krachten van de auto in bullet goed
	//draai auto in bullet
	return 1;//succes!
}

void App::init(void)
{
	wand.init("WandPosition");
	head.init("MainUserHead");
	leftButton.init("LeftButton");
	upArrow.init("UpArrow"); downArrow.init("DownArrow"); leftArrow.init("LeftArrow"); rightArrow.init("RightArrow");
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);

	bullet3Init();
	brickwall_texture = CaveLib::loadTexture("data/CellStroll/textures/brickwall.jpg");
	checkers_model = CaveLib::loadModel("data/CellStroll/models/checkers_sphere.obj", new ModelLoadOptions(1.0f));
	camera = new Camera();
}

int App::bullet3Init()
{
	broadphase = new btDbvtBroadphase();
	collisionConfiguration = new btDefaultCollisionConfiguration();
	dispatcher = new btCollisionDispatcher(collisionConfiguration);
	solver = new btSequentialImpulseConstraintSolver();
	world = new btDiscreteDynamicsWorld(dispatcher, broadphase, solver, collisionConfiguration);
	world->setGravity(btVector3(0, -10, 0));
	car.initCar();
	world->addRigidBody(car.thisVehicle);
	return 1;
}

void App::preFrame(double frameTime, double totalTime)
{
	// timer
	clock_t clock_end = clock();
	GLfloat timeFctr = GLfloat(clock_end - clock_start) / CLOCKS_PER_SEC; // calculate time(s) elapsed since last frame
	camera->tf = timeFctr;
	fps = int(1 / timeFctr);
	clock_start = clock();

	// camera
	if (upArrow.getData() == ON)
		camera->updateSpeed();
	if (leftArrow.getData() == ON)
		camera->rotateCamYaw(-1.0f);
	else if (rightArrow.getData() == ON)
		camera->rotateCamYaw(1.0f);

	// wand
	glm::mat4 mat = wand.getData();
}

void App::draw(const glm::mat4 &projectionMatrix, const glm::mat4 &modelViewMatrix)
{
	glEnable(GL_TEXTURE_2D);
	camera->refresh();
	glScalef(100.0f, 100.0f, 100.0f);

	checkers_model->draw();
	DrawWireFrame();
	btCollisionObject*	colObj = world->getCollisionObjectArray()[0];
	printf("");
}

void App::DrawWireFrame(void)
{
	glPushMatrix();
	glLineWidth(1.0);
	glColor3f(1.0, 0.0, 0.0);
	glTranslatef(0.0f, -0.05f, 0.0f);
	for (float i = -2.0f; i <= 2.0f; i+=0.1f)
	{
		glBegin(GL_LINES);
		glVertex3f(i, 0.0f, -2.0f);
		glVertex3f(i, 0.0f, 2.0f);
		glEnd();
		glBegin(GL_LINES);
		glVertex3f(-2.0f, 0.0f, i);
		glVertex3f(2.0f, 0.0f, i);
		glEnd();
	}
	glPopMatrix();
}