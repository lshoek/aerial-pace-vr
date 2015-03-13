#pragma comment(lib,"opengl32")
#include <GL\glew.h>
#include <Windows.h>
#include "App.h"
#include <CaveLib\CaveLib.h>
#include <CaveLib\texture.h>
#include <CaveLib\model.h>

App::App(WiiMoteWrapper * w){
	wiiMoteWrapper = w; 
}
App::~App(void){
	
}

int App::updateCarSpeed(GLfloat timeFactor){
	
	if (!wiiMoteWrapper || !physics.world)
		return -2;
	if (wiiMoteWrapper->buttonHome)//pauze, zolang home is ingedrukt
		return -1;
	float newSpeed = 0;
	if (wiiMoteWrapper->buttonOne)//speed goes down
	{
		newSpeed = car.MAXFORCE * timeFactor;
	}
	if (wiiMoteWrapper->buttonTwo)//speed goes up
	{
		newSpeed = car.MAXFORCE * timeFactor;
	}
	car.steeringWheelDegrees = wiiMoteWrapper->degrees;
	car.carSpeed += newSpeed+1;
	car.carSpeed *= 0.91f;
	car.carDegrees = (car.carDegrees + car.steeringWheelDegrees);
	float carRadians = (car.carDegrees)*3.14 / 180;
	btVector3 physicsSpeed(car.carSpeed * cos(carRadians) - 0 * sin(carRadians),
		car.carSpeed * sin(carRadians) - 0 * cos(carRadians),
		0);
	physics.realCar->setGravity(physicsSpeed);
	physics.world->stepSimulation(timeFactor);//en updaten
	//debug
	auto a = physics.realCar->getWorldTransform();
	return 1;//succes!
}

void App::init(void)
{
	//wand.init("WandPosition");
	//head.init("MainUserHead");
	leftButton.init("LeftButton");
	//upArrow.init("UpArrow"); downArrow.init("DownArrow"); leftArrow.init("LeftArrow"); rightArrow.init("RightArrow");
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	physics.bullet3Init();
	brickwall_texture = CaveLib::loadTexture("data/CellStroll/textures/brickwall.jpg");
	checkers_model = CaveLib::loadModel("data/CellStroll/models/checkers_sphere.obj", new ModelLoadOptions(1.0f));
	camera = new Camera();
}

void App::preFrame(double frameTime, double totalTime)
{
	// timer
	clock_t clock_end = clock();
	GLfloat timeFctr = GLfloat(clock_end - clock_start) / CLOCKS_PER_SEC; // calculate time(s) elapsed since last frame
	//camera->tf = timeFctr;
	fps = int(1 / timeFctr);
	clock_start = clock();
	updateCarSpeed(timeFctr);
	//camera
	/*if (upArrow.getData() == ON)
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
	}*/

	// wand
	//glm::mat4 mat = wand.getData();
}

void App::draw(const glm::mat4 &projectionMatrix, const glm::mat4 &modelViewMatrix)
{
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_COLOR_MATERIAL);
	//std::cout << fps << "\n";
	//glTranslatef(-1.0f, 0.0f, -1.0f);
	//glScalef(5.0f, 5.0f, 5.0f);

	//camera->refresh();
	glScalef(100.0f, 100.0f, 100.0f);
	checkers_model->draw();
	DrawBrickWall();
	//voorbeeld: auto a =physics.world->getCollisionObjectArray()[0];
	printf("");
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