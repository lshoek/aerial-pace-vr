#include "Physics.h"


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

int Physics::bullet3Init(){
	broadphase = new btDbvtBroadphase();
	collisionConfiguration = new btDefaultCollisionConfiguration();
	dispatcher = new btCollisionDispatcher(collisionConfiguration);
	solver = new btSequentialImpulseConstraintSolver();
	world = new btDiscreteDynamicsWorld(dispatcher, broadphase, solver, collisionConfiguration);
	world->setGravity(btVector3(0, 0, 0));
	//addFloor(-1.0,-1.0,-1.0);
	addCar();
	return 1;
}

void Physics::addFloor(float x1, float x2, float x3){
	btBoxShape* pBoxShape = new btBoxShape(btVector3(3.0f, 0.05f, 3.0f));
	// give our box an initial position of (0,0,0)
	btTransform transform;
	transform.setIdentity();
	transform.setOrigin(btVector3(x1,x2,x3));
	// create a motion state
	btMotionState* m_pMotionState = new btDefaultMotionState(transform);
	// create the rigid body construction info object, giving it a 
	// mass of 1, the motion state, and the shape
	btRigidBody::btRigidBodyConstructionInfo rbInfo(0, m_pMotionState, pBoxShape);
	floorParts.push_back(new btRigidBody(rbInfo));

	for each (btRigidBody* btb in floorParts)
	{
		world->addRigidBody(btb);
	}
}

void Physics::addCar(){
	// create a box shape of size (1,1,1)
	btBoxShape* pBoxShape = new btBoxShape(btVector3(1.0f, 1.0f, 1.0f));
	// give our box an initial position of (0,0,0)
	btTransform transform;
	transform.setIdentity();
	transform.setOrigin(btVector3(0.0f, 0.0f, 0.0f));
	// create a motion state
	btMotionState* m_pMotionState = new btDefaultMotionState(transform);
	// create the rigid body construction info object, giving it a 
	// mass of 1, the motion state, and the shape
	btRigidBody::btRigidBodyConstructionInfo rbInfo(1.0f, m_pMotionState, pBoxShape);
	realCar = new btRigidBody(rbInfo);
	world->addRigidBody(realCar);
	// inform our world that we just created a new rigid body for 
	// it to manage
}