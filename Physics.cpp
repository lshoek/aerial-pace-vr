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

	for each (btRigidBody* btb in floorParts)
	{
		world->addRigidBody(btb);
	}
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
	btQuaternion newRotation = btQuaternion(btVector3(0, 1.0f, 0), 0);
	
	realCar->getWorldTransform().setRotation(newRotation);
	//realCar->setGravity(btVector3(0, -10, 0));
	realCar->setFriction(0.1);
	world->addRigidBody(realCar);
	// inform our world that we just created a new rigid body for 
	// it to manage
}