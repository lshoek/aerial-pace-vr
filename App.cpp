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
	
	if (wiiMoteWrapper->buttonHome)//pauze, zolang home is ingedrukt
		return 0;
	GLfloat acceleration = 0;//extraspeed 
	GLfloat constspeed = 10;//de snelheid is altijd 10m/s
	GLfloat deltaspeed = constspeed * timeFactor;
	if (wiiMoteWrapper->buttonOne)//speed goes down
	{
		acceleration -= deltaspeed;
	}
	if (wiiMoteWrapper->buttonTwo)//speed goes up
	{
		acceleration += deltaspeed;
	}
	//de auto moet versnellen met de acceleratie

	//laat bullet de zwaartekracht berekenen voor een mogelijke extra snelheid

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

int App::bullet3Init(){
	broadphase = new btDbvtBroadphase();
	collisionConfiguration = new btDefaultCollisionConfiguration();
	dispatcher = new btCollisionDispatcher(collisionConfiguration);
	solver = new btSequentialImpulseConstraintSolver();
	world = new btDiscreteDynamicsWorld(dispatcher, broadphase, solver, collisionConfiguration);
	world->setGravity(btVector3(0, 0, 0));
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

	//camera
	if (upArrow.getData() == ON)
		camera->updateSpeed();

	if (leftArrow.getData() == ON)
	{
		camera->rotateCamYaw(-1.0f);
		//if (last_player_index <= 1)
		//	player_index = 0;
	}
	else if (rightArrow.getData() == ON)
	{
		camera->rotateCamYaw(1.0f);
		//if (last_player_index >= 1)
		//	player_index = 2;
	}

	// wand
	glm::mat4 mat = wand.getData();
}

void App::draw(const glm::mat4 &projectionMatrix, const glm::mat4 &modelViewMatrix)
{
	glEnable(GL_TEXTURE_2D);
	//glTranslatef(-1.0f, 0.0f, -1.0f);
	//glScalef(5.0f, 5.0f, 5.0f);

	camera->refresh();
	glScalef(100.0f, 100.0f, 100.0f);
	checkers_model->draw();
	DrawBrickWall();
}

void App::DrawBrickWall(void)
{
	glBindTexture(GL_TEXTURE_2D, brickwall_texture->tid());
	glPushMatrix();
	glBegin(GL_QUADS);

	// Top 
	glTexCoord3f(1.0f, 0.0f, 0.0f); glVertex3f(1.0f, 1.0f, -1.0f);
	glTexCoord3f(1.0f, 1.0f, 0.0f); glVertex3f(-1.0f, 1.0f, -1.0f);
	glTexCoord3f(0.0f, 1.0f, 1.0f); glVertex3f(-1.0f, 1.0f, 1.0f);
	glTexCoord3f(0.0f, 0.0f, 1.0f); glVertex3f(1.0f, 1.0f, 1.0f);

	// Bottom 
	glTexCoord3f(1.0f, 0.0f, 0.0f); glVertex3f(1.0f, -1.0f, 1.0f);
	glTexCoord3f(1.0f, 1.0f, 0.0f); glVertex3f(-1.0f, -1.0f, 1.0f);
	glTexCoord3f(0.0f, 1.0f, 1.0f); glVertex3f(-1.0f, -1.0f, -1.0f);
	glTexCoord3f(0.0f, 0.0f, 1.0f); glVertex3f(1.0f, -1.0f, -1.0f);

	// Front 
	glTexCoord3f(1.0f, 0.0f, 0.0f); glVertex3f(1.0f, 1.0f, 1.0f);
	glTexCoord3f(1.0f, 1.0f, 0.0f); glVertex3f(-1.0f, 1.0f, 1.0f);
	glTexCoord3f(0.0f, 1.0f, 1.0f); glVertex3f(-1.0f, -1.0f, 1.0f);
	glTexCoord3f(0.0f, 0.0f, 1.0f); glVertex3f(1.0f, -1.0f, 1.0f);

	// Back face
	glTexCoord3f(1.0f, 0.0f, 0.0f); glVertex3f(1.0f, -1.0f, -1.0f);
	glTexCoord3f(1.0f, 1.0f, 0.0f); glVertex3f(-1.0f, -1.0f, -1.0f);
	glTexCoord3f(0.0f, 1.0f, 1.0f); glVertex3f(-1.0f, 1.0f, -1.0f);
	glTexCoord3f(0.0f, 0.0f, 1.0f); glVertex3f(1.0f, 1.0f, -1.0f);

	// Left face 
	glTexCoord3f(1.0f, 0.0f, 0.0f); glVertex3f(-1.0f, 1.0f, 1.0f);
	glTexCoord3f(1.0f, 1.0f, 0.0f); glVertex3f(-1.0f, 1.0f, -1.0f);
	glTexCoord3f(0.0f, 1.0f, 1.0f); glVertex3f(-1.0f, -1.0f, -1.0f);
	glTexCoord3f(0.0f, 0.0f, 1.0f); glVertex3f(-1.0f, -1.0f, 1.0f);

	// Right face
	glTexCoord3f(1.0f, 0.0f, 0.0f); glVertex3f(1.0f, 1.0f, -1.0f);
	glTexCoord3f(1.0f, 1.0f, 0.0f); glVertex3f(1.0f, 1.0f, 1.0f);
	glTexCoord3f(0.0f, 1.0f, 1.0f); glVertex3f(1.0f, -1.0f, 1.0f);
	glTexCoord3f(0.0f, 0.0f, 1.0f); glVertex3f(1.0f, -1.0f, -1.0f);
	glEnd();
	glPopMatrix();
}