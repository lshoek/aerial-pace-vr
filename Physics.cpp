#include "Physics.h"
#include <functional>
#include <math.h>

const float Physics::MAXFORCE = 300.0f;

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
}

int Physics::bullet3Init(){
	broadphase = new btDbvtBroadphase();
	collisionConfiguration = new btDefaultCollisionConfiguration();
	dispatcher = new btCollisionDispatcher(collisionConfiguration);
	solver = new btSequentialImpulseConstraintSolver();
	world = new btDiscreteDynamicsWorld(dispatcher, broadphase, solver, collisionConfiguration);
	world->setGravity(btVector3(0,-10,0));
	addCar();
	return 1;
}

void Physics::addFloor(cModel* stage){
	std::vector<glm::vec3> positions;
	std::function<void(cModel::cSkeletonBone*)> walkObject;
	walkObject = [&](cModel::cSkeletonBone* b)
	{
		if (b->mesh){
			std::vector<glm::vec3> meshPositions = b->mesh->getVertices();

			positions.insert(positions.end(), meshPositions.begin(), meshPositions.end());
		}
		for (auto bb : b->children)
			walkObject(bb);
	};
	walkObject(stage ->rootBone);
	btVector3* gVertices = new btVector3[positions.size()];
	int* gIndices = new int[positions.size()];
	for (size_t i = 0; i < positions.size(); i++)
	{
		gVertices[i] = btVector3(positions[i].x, positions[i].y, positions[i].z);
		gIndices[i] = i;
	}
	//fill
	btTriangleIndexVertexArray* m_indexVertexArrays = new btTriangleIndexVertexArray(positions.size() / 3,
		gIndices,
		3 * sizeof(int),
		positions.size(), (btScalar*)&gVertices[0].x(), sizeof(btVector3));
	btVector3 aabbMin(-100000, -100000, -100000), aabbMax(100000, 100000, 100000);
	levelShape = new btBvhTriangleMeshShape(m_indexVertexArrays,true, aabbMin, aabbMax);
	//btCollisionShape* colshape = new btStaticPlaneShape(btVector3(1, 1, 1), 1);
	btMotionState* m_pMotionState = new btDefaultMotionState(btTransform(btQuaternion(0, 0, 0, 1), btVector3(0, 0, 0)));
	btRigidBody::btRigidBodyConstructionInfo rbInfo(0, m_pMotionState, levelShape);
	level = new btRigidBody(rbInfo);
	world->addCollisionObject(level);
	level->activate();
}

void Physics::addCar(){
	if (!realCar){
		world->removeRigidBody(realCar);
		delete realCar;
	}
	float mass = 1.0f;//kg
	btVector3 fallInertia;
	btBoxShape* pBoxShape = new btBoxShape(btVector3(1,1,1));
	btMotionState* m_pMotionState = new btDefaultMotionState(btTransform(btQuaternion(0, 0, 0, 1), btVector3(-8.6f, 5,7)));
	pBoxShape->calculateLocalInertia(mass, fallInertia);
	btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, m_pMotionState, pBoxShape, fallInertia);
	realCar = new btRigidBody(rbInfo);
	btQuaternion newRotation = btQuaternion(btVector3(0, 1.0f, 0), btRadians(0));	
	realCar->getWorldTransform().setRotation(newRotation);
	realCar->setFriction(0.5);
	world->addRigidBody(realCar);
}

void Physics::updateCar(float angle, WiiMoteWrapper* wiiMoteWrapper)
{
	float deltaspeed = 0;
	if (wiiMoteWrapper->buttonOne){//speed goes down
		deltaspeed -= MAXFORCE * timeFctr;
	}
	if (wiiMoteWrapper->buttonTwo){//speed goes up
		deltaspeed += MAXFORCE * timeFctr;
	}
	btVector3 deltaposition(deltaspeed, 0, deltaspeed);
	float rotationFactor = btRadians(wiiMoteWrapper->degrees);
	deltaposition = deltaposition.rotate(btVector3(0, 1, 0), rotationFactor);
	//realCar->applyTorque(btVector3(0, rotationFactor, 0));
	//float rotationFactor = btRadians(wiiMoteWrapper->degrees);
	//realCar->applyTorque(btVector3(0, tempAngle-angle, 0));
	tempAngle = angle;
	float mvpRaw[16];
	realCar->getCenterOfMassTransform().getOpenGLMatrix(mvpRaw);
	glm::mat4 carmvp = glm::make_mat4(mvpRaw);
	glm::vec4 p1 = glm::vec4(0, 0, 0, 1)*carmvp;
	glm::vec4 p2 = glm::vec4(1, 0, 0, 1)*carmvp;
	btVector3 p3(p2.x - p1.x, 0, p2.z - p1.z);
	//p3 = btVector3(p3.x()*cos(rotationFactor)+p3.z()*sin(rotationFactor), 0, -p3.x()*sin(rotationFactor)+p3.z()*cos(rotationFactor));
	//deltaposition *= p3;
	realCar->applyCentralForce(deltaposition);//.rotate(btVector3(0, 1.0, 0), btRadians(wiiMoteWrapper->degrees))
	//realCar->applyForce(deltaposition*p3,btVector3(0.9,0,0)*p3);//.rotate(btVector3(0, 1.0, 0), btRadians(wiiMoteWrapper->degrees))
	realCar->activate();
	world->stepSimulation(timeFctr);//en updaten
	btVector3 b2 = realCar->getWorldTransform().getOrigin();
	//printf("auto %f,%f,%f :%f rad %d\n", b2.x(), b2.y(), b2.z(), rotationFactor, realCar->getCollisionFlags());
}

void Physics::updateTimeFactor(float fctr)
{
	timeFctr = fctr;
}