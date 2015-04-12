#pragma once
#ifdef _DEBUG
#pragma comment(lib, "Bullet3Common_vs2010_debug.lib")
#pragma comment(lib, "Bullet3Collision_vs2010_debug.lib")
#pragma comment(lib, "Bullet3Dynamics_vs2010_debug.lib")
#pragma comment(lib, "BulletCollision_vs2010_debug.lib")
#pragma comment(lib, "BulletDynamics_vs2010_debug.lib")
#pragma comment(lib, "Bullet3Geometry_vs2010_debug.lib")
#pragma comment(lib, "LinearMath_vs2010_debug.lib")
#else
#pragma comment(lib, "Bullet3Common_vs2010.lib")
#pragma comment(lib, "Bullet3Collision_vs2010.lib")
#pragma comment(lib, "Bullet3Dynamics_vs2010.lib")
#pragma comment(lib, "BulletCollision_vs2010.lib")
#pragma comment(lib, "BulletDynamics_vs2010.lib")
#pragma comment(lib, "Bullet3Geometry_vs2010.lib")
#pragma comment(lib, "LinearMath_vs2010.lib")
#endif
#include <btBulletDynamicsCommon.h>
#include <glm/gtc/type_ptr.hpp>
#include <vector>
#include <CaveLib\model.h>
#include "WiiYourself\WiiMoteWrapper.h"


using namespace std;

class Physics
{
public:
	Physics();
	~Physics();

	btBroadphaseInterface*                  broadphase;
	btDefaultCollisionConfiguration*        collisionConfiguration;
	btCollisionDispatcher*                  dispatcher;
	btSequentialImpulseConstraintSolver*    solver;
	btDiscreteDynamicsWorld*                world;
	btBvhTriangleMeshShape*					levelShape;
	btCollisionObject *						level;
	btRigidBody *							realCar;

	int Physics::bullet3Init();
	void addCar();
	void addFloor(cModel* stage);
	void updateCar(float, WiiMoteWrapper *);
	void updateTimeFactor(float);

	static const float MAXFORCE;	
	float timeFctr = 0.1f;
};

