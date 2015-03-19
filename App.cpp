#pragma comment(lib,"opengl32")
#define _USE_MATH_DEFINES
#include <GL\glew.h>
#include <Windows.h>
#include "App.h"
#include "Physics.h"
#include <CaveLib\model.h>

App::App(WiiMoteWrapper * w){
	wiiMoteWrapper = w; 
	car.initCar(w, &physics);
}
App::~App(void){}


/*nog niet weghalen
int App::updateCarSpeed(GLfloat timeFactor){
	
	if (!wiiMoteWrapper || !physics.world)
		return -2;
	if (wiiMoteWrapper->buttonHome)//pauze, zolang home is ingedrukt
		return -1;
	float newSpeed = 0;
	if (wiiMoteWrapper->buttonOne)//speed goes down
		newSpeed -= car.MAXFORCE * timeFactor;
	if (wiiMoteWrapper->buttonTwo)//speed goes up
		newSpeed += car.MAXFORCE * timeFactor;
	newSpeed += car.MAXFORCE;
	btVector3 deltaSpeed = btVector3(newSpeed, 0, 0);// .rotate(btVector3(0, 1, 0), btRadians(wiiMoteWrapper->degrees));
	float r = (btRadians(wiiMoteWrapper->degrees) + car.carRadians);
	car.carSpeed += newSpeed;
	car.carRadians = r;
	btQuaternion rotation = physics.realCar->getOrientation();
	float rotationAngle = rotation.getAngle() + btRadians(30.0f);//wiiMoteWrapper->degrees
	btQuaternion newRotation = btQuaternion(btVector3(0,1.0f,0), rotationAngle);
	//newRotation *= rotation;
	physics.realCar->getWorldTransform().setRotation(newRotation);
	car.carRadians = newRotation.getAngle();
	//physics.realCar->applyCentralForce(deltaSpeed);
	physics.realCar->applyTorque(deltaSpeed);
	physics.realCar->activate();
	physics.world->stepSimulation(timeFactor);//en updaten	
	btVector3 b = physics.realCar->getWorldTransform().getOrigin();
	printf("auto %f,%f,%f :%f rad %f m/s\n", b.x(), b.y(), b.z(), physics.realCar->getOrientation().getAngle(), car.carSpeed);
	return 1;//succes!
}*/

void App::init(void)
{
	//upArrow.init("UpArrow"); downArrow.init("DownArrow"); leftArrow.init("LeftArrow"); rightArrow.init("RightArrow");
	physics.bullet3Init();
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
	//updateCarSpeed(timeFctr);
	//testUpdate(timeFctr);
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
	
	glPushMatrix();
	camera->refresh();
	btVector3 b = physics.realCar->getWorldTransform().getOrigin();
	b = car.direction;
	glTranslatef(b.x(), 0, b.z());
	//glTranslatef()
	checkers_model->bbox.bounds;
	float angle = physics.realCar->getOrientation().getAngle();
	glRotatef(car.carRadians, 0, 1, 0);
	checkers_model->draw();
	glPopMatrix();
	//setworldtransform
	for each (btRigidBody* floor in physics.floorParts)
	{
		btVector3 b = floor->getWorldTransform().getOrigin();
		glPushMatrix();
		glTranslatef(b.x(), b.y(), b.z());
		glColor3f(0.3, 0.5, 0.7);
		glBegin(GL_POLYGON);
		glVertex3f(1.0, 0, -0);
		glVertex3f(1.0, 0, 1.0);
		glVertex3f(-0, 0, 1.0);
		glVertex3f(-0, 0, -0);
		glEnd();
		//DrawWireFrame();
		glPopMatrix();
	}
	
	//classicFont->drawText("HELLO WORLD!", 10.0f, 10.0f, 0.0f);
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
