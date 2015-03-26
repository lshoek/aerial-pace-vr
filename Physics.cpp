#include "Physics.h"

const float Physics::MAXFORCE = 100.0f;

Physics::Physics()
{
}

Physics::~Physics()
{
	delete world;
	delete solver;
	delete collisionConfiguration;
	delete dispatcher;
	delete broadphase;
	floorParts.clear();
}

int Physics::bullet3Init(WiiMoteWrapper* w){
	wiiMoteWrapper = w;
	broadphase = new btDbvtBroadphase();
	collisionConfiguration = new btDefaultCollisionConfiguration();
	dispatcher = new btCollisionDispatcher(collisionConfiguration);
	solver = new btSequentialImpulseConstraintSolver();
	world = new btDiscreteDynamicsWorld(dispatcher, broadphase, solver, collisionConfiguration);
	world->setGravity(btVector3(0, 0, 0));
	world->setGravity(btVector3(0,-10,0));
	addFloor(btVector3(100, 1, 100), btVector3(-50, -2, 50));

	addCar();
	return 1;
}

void Physics::addFloor(const btVector3 &size, const btVector3 &origin){
	btBoxShape* pBoxShape = new btBoxShape(btVector3(size));
	btTransform transform;
	transform.setIdentity();
	transform.setOrigin(origin);
	// create a motion state
	btMotionState* m_pMotionState = new btDefaultMotionState(transform);
	// create the rigid body construction info object, giving it a 
	// mass of 1, the motion state, and the shape
	btRigidBody::btRigidBodyConstructionInfo rbInfo(0, m_pMotionState, pBoxShape);
	floorParts.push_back(new btRigidBody(rbInfo));
	world->addRigidBody(floorParts.back());
}

void Physics::addCar(){
	float mass = 1.0f;//kg
	btVector3 fallInertia;
	// create a box shape of size (1,1,1)
	btBoxShape* pBoxShape = new btBoxShape(btVector3(1.0f, 1.0f, 1.0f));
	// give our box an initial position of (0,0,0)
	btTransform transform;
	transform.setIdentity();
	transform.setOrigin(btVector3(0.0f, 0.0f, 0.0f));
	// create a motion state
	btMotionState* m_pMotionState = new btDefaultMotionState(btTransform(btQuaternion(0, 0, 0, 1), btVector3(0, 0,0)));
	// create the rigid body construction info object, giving it a 
	// mass of 1, the motion state, and the shape
	pBoxShape->calculateLocalInertia(mass, fallInertia);
	btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, m_pMotionState, pBoxShape,fallInertia);
	realCar = new btRigidBody(rbInfo);
	btQuaternion newRotation = btQuaternion(btVector3(0, 1.0f, 0), btRadians(0));
	
	realCar->getWorldTransform().setRotation(newRotation);
	realCar->setFriction(0.1);
	world->addRigidBody(realCar);
	// inform our world that we just created a new rigid body for 
	// it to manage
}

void Physics::updateCar(float timeFactor){
	float deltaspeed = 0;
	if (wiiMoteWrapper->buttonOne){//speed goes down
		deltaspeed -= MAXFORCE * timeFactor;
	}
	if (wiiMoteWrapper->buttonTwo){//speed goes up
		deltaspeed += MAXFORCE * timeFactor;
	}
	btVector3 deltaposition(deltaspeed, 0, deltaspeed);
	float rotationFactor = btRadians(wiiMoteWrapper->degrees);
	realCar->applyTorque(btVector3(0, rotationFactor, 0));
	float mvpRaw[16];
	realCar->getCenterOfMassTransform().getOpenGLMatrix(mvpRaw);
	glm::mat4 carmvp = glm::make_mat4(mvpRaw);
	glm::vec4 p1 = glm::vec4(0, 0, 0, 1)*carmvp;
	glm::vec4 p2 = glm::vec4(1, 0, 0, 1)*carmvp;
	btVector3 p3(p2.x - p1.x, 0, p2.z - p1.z);
	realCar->applyCentralForce(deltaposition*p3);//.rotate(btVector3(0, 1.0, 0), btRadians(wiiMoteWrapper->degrees))
	realCar->activate();
	world->stepSimulation(timeFactor);//en updaten	

	btVector3 b2 = realCar->getWorldTransform().getOrigin();
	printf("auto %f,%f,%f :%f rad \n", b2.x(), b2.y(), b2.z(), rotationFactor);
}