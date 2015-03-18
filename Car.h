
#pragma once

#include <btBulletDynamicsCommon.h>
class Car
{
public:
	Car();
	~Car();

	void initCar();
	//graden van wielen
	float steeringWheelDegrees;
	//graden van auto
	float carRadians;
	//snelheids van auto
	float carSpeed;
	//maximale snelheid
	float MAXFORCE;
	btVector3 direction;
};

