#pragma comment(lib,"opengl32")
#define _USE_MATH_DEFINES
#include <GL\glew.h>
#include <Windows.h>
#include "App.h"
#include "Physics.h"
#include <CaveLib\model.h>

App::App(WiiMoteWrapper * w){
	wiiMoteWrapper = w; 
}
App::~App(void){}



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
	btVector3 physicsSpeed(car.carSpeed, 0, 0);
	physicsSpeed.rotate(btVector3(0, 1, 0), car.carRadians);
	//gewoon rechtdoor gaan
	//wiiMoteWrapper->degrees = 5;
	//newSpeed += car.MAXFORCE * timeFactor;
	btVector3 deltaSpeed(newSpeed, 0, 0);
	deltaSpeed.rotate(btVector3(0, 1, 0), btRadians(wiiMoteWrapper->degrees));
	physicsSpeed += deltaSpeed;
	physicsSpeed *= 0.9f;//zoiets
	//float r = fmod(btRadians(wiiMoteWrapper->degrees) + car.carRadians,2*M_PI);
	float r = (btRadians(wiiMoteWrapper->degrees) + car.carRadians);
	//btClamp<float>(r, -M_PI*2, M_PI*2);
	//physicsSpeed = physicsSpeed.rotate(btVector3(0, 1, 0), r);
	car.carSpeed += newSpeed;
	btClamp<float>(car.carSpeed, car.MAXFORCE * 3 / 10, car.MAXFORCE);
	car.carRadians = r;
	car.direction = physicsSpeed;
	
	physicsSpeed.setY(-9.8f);
	//physics.realCar->clearForces();

	//physics.realCar->setGravity(physicsSpeed);
	physics.world->stepSimulation(timeFactor);//en updaten
	//debug
	btTransform a = physics.realCar->getWorldTransform();
	btVector3 transform = a.getOrigin();
	float length = transform.length();
	return 1;//succes!
}

void App::testUpdate(GLfloat timeFactor){
	//nieuw!
	float newSpeed = 0;
	if (wiiMoteWrapper->buttonOne)//speed goes down
		newSpeed -= car.MAXFORCE * timeFactor;
	if (wiiMoteWrapper->buttonTwo)//speed goes up
		newSpeed += car.MAXFORCE * timeFactor;
	btVector3 newForce(newSpeed, 0, 0);
	newForce = newForce.rotate(btVector3(0, 1.0, 0), btRadians(wiiMoteWrapper->degrees));

	btTransform tr = physics.realCar->getWorldTransform();
	btQuaternion quat(btVector3(0, 1.0, 0), btRadians(wiiMoteWrapper->degrees));
	btQuaternion oldQuat = physics.realCar->getOrientation();
	//quat.setRotation(btVector3(0, 1.0, 0), btRadians(wiiMoteWrapper->degrees)); //or quat.setEulerZYX depending on the ordering you want
	quat *= oldQuat;
	tr.setRotation(quat);
	float o1 = oldQuat.getAngle();
	btVector3 a1 = oldQuat.getAxis();
	float o2 = quat.getAngle();
	btVector3 a2 = quat.getAxis();
	physics.realCar->setFriction(0.7);	
	physics.realCar->setWorldTransform(tr);
	physics.realCar->applyCentralImpulse(newForce);
	physics.realCar->activate();
	physics.world->stepSimulation(timeFactor);
}

void App::init(void)
{
	upArrow.init("UpArrow"); downArrow.init("DownArrow"); leftArrow.init("LeftArrow"); rightArrow.init("RightArrow");
	physics.bullet3Init();
	//classicFont = new Font("data/aerial-pace-vr/fonts/classicfnt32.fnt", "data/aerial-pace-vr/fonts/classicfnt32.png");
	//checkers_model = CaveLib::loadModel("data/aerial-pace-vr/models/checkers_sphere.obj", new ModelLoadOptions(1.0f));
	//camera = new Camera();
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
}

void App::preFrame(double frameTime, double totalTime)
{
	// timer
	clock_t clock_end = clock();
	GLfloat timeFctr = GLfloat(clock_end - clock_start) / CLOCKS_PER_SEC; // calculate time(s) elapsed since last frame
	//camera->tf = timeFctr;
	fps = int(1 / timeFctr);
	clock_start = clock();
	//updateCarSpeed(timeFctr);
	testUpdate(timeFctr);

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
	//camera->refresh();
	btVector3 b = physics.realCar->getWorldTransform().getOrigin();
	printf("auto %f,%f,%f :%f rad %f m/s\n", b.x(), b.y(), b.z(), physics.realCar->getOrientation().getAngle(), car.carSpeed);
	glTranslatef(b.x(), 0, b.z());
	glRotatef(physics.realCar->getOrientation().getAngle(), 0, 1, 0);
	glScalef(0.5, 0.5, 0.5);
	glBegin(GL_POLYGON);
	glColor3f(1.0, 1.0, 0);
	glVertex3f(1.0, 0, 1.0);
	glVertex3f(1.0, 1, 1.0);
	glVertex3f(0, 1, 1.0);
	glVertex3f(0, 0, 1.0);
	glEnd();

	/*// Purple side - RIGHT
	glBegin(GL_POLYGON);
	glColor3f(1.0, 0.0, 1.0);
	glVertex3f(1.0, 0, 0);
	glVertex3f(1.0, 1.0, 0);
	glVertex3f(1.0, 1.0, 1.0);
	glVertex3f(1.0, 0, 1.0);
	glEnd();

	// Green side - LEFT
	glBegin(GL_POLYGON);
	glColor3f(0.0, 1.0, 0.0);
	glVertex3f(0, 0, 1.0);
	glVertex3f(-0, 1.0, 1.0);
	glVertex3f(-0, 1.0, 0);
	glVertex3f(-0, 0, -0);
	glEnd();

	// Blue side - TOP
	glBegin(GL_POLYGON);
	glColor3f(0.0, 0.0, 1.0);
	glVertex3f(1.0, 1.0, 1.0);
	glVertex3f(1.0, 1.0, -0);
	glVertex3f(-0, 1.0, -0);
	glVertex3f(-0, 1.0, 1.0);
	glEnd();*/

	// Red side - BOTTOM
	glBegin(GL_POLYGON);
	glColor3f(1.0, 0.0, 0.0);
	glVertex3f(1.0, 0, -0);
	glVertex3f(1.0, 0, 1.0);
	glVertex3f(-0, 0, 1.0);
	glVertex3f(-0, 0, -0);
	glEnd();
	//glScalef(0.3, 0.3, 0.3);
	//gltrans
	//checkers_model->draw();
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
