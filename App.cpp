#pragma comment(lib,"opengl32")
#define _USE_MATH_DEFINES
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
		newSpeed -= car.MAXFORCE * timeFactor;
	}
	if (wiiMoteWrapper->buttonTwo)//speed goes up
	{
		newSpeed += car.MAXFORCE * timeFactor;
	}
	btVector3 physicsSpeed(car.carSpeed, 0, 0);
	//gewoon rechtdoor gaan
	//wiiMoteWrapper->degrees = 5;
	btVector3 deltaSpeed(newSpeed, 0, 0);
	physicsSpeed += deltaSpeed;
	//physicsSpeed *= 0.9f;//zoiets
	//float r = fmod(btRadians(wiiMoteWrapper->degrees) + car.carRadians,2*M_PI);
	float r = (btRadians(wiiMoteWrapper->degrees) + car.carRadians);
	physicsSpeed = physicsSpeed.rotate(btVector3(0, 1, 0), r);
	car.carSpeed += newSpeed;
	car.carRadians = r;
	car.direction = physicsSpeed;
	physicsSpeed.setY(-9.8f);
	physics.realCar->setGravity(physicsSpeed);
	physics.world->stepSimulation(timeFactor);//en updaten
	//debug
	btTransform a = physics.realCar->getWorldTransform();
	btVector3 transform = a.getOrigin();
	float length = transform.length();
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
		camera->rotateCamYaw(-1.0f);
	else if (rightArrow.getData() == ON)
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
	glPushMatrix();
	glScalef(100.0f, 100.0f, 100.0f);
	DrawWireFrame();
	glPopMatrix();
	//checkers_model->draw();
	//voorbeeld: auto a =physics.world->getCollisionObjectArray()[0];
	
	
	glPushMatrix();
	btTransform a = physics.realCar->getWorldTransform();
	//glm::mat4 
	btTransform btf = physics.realCar->getCenterOfMassTransform();
	btVector3 b = physics.realCar->getWorldTransform().getOrigin();
	printf("auto %f,%f,%f :%f rad %f m/s\n", b.x(), b.y(), b.z(), car.carRadians, car.carSpeed);
	glTranslatef(b.x(), b.y(), b.z());
	glRotatef(car.carRadians, 0, 1, 0);
	//glScalef(0.3, 0.3, 0.3);
	//gltrans
	checkers_model->draw();
	glPopMatrix();
	//setworldtransform
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