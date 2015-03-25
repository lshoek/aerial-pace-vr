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
#include <vector>

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

	int Physics::bullet3Init();
	void addCar();
	void addFloor(const btVector3 &size, const btVector3 &origin);
	btRigidBody * realCar;
	vector<btRigidBody *> floorParts;
};

