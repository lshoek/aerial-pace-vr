#pragma once

#include <btBulletDynamicsCommon.h>
#include <glm/gtc/type_ptr.hpp>
#include "WiiYourself\WiiMoteWrapper.h"
#include "Physics.h"

class Car
{
public:
	Car();
	~Car();

	void initCar(WiiMoteWrapper* w, Physics* p);

	void updateCar(float timeFactor);

	//graden van auto
	int carDegrees;
	//snelheid van auto
	float carSpeed;
	//maximale snelheid
	float MAXFORCE;
	btVector3 direction;
	btVector3 speedVector;
	btVector3 position;
	WiiMoteWrapper * wiiMoteWrapper;
	Physics * physics;
};