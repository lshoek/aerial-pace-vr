#include "Car.h"

Car::Car()
{
	carSpeed = steeringWheelDegrees = 0;
	carRadians = 0;
	MAXFORCE = 5.0f;

	direction = btVector3(0, 0, 0);
}

void Car::initCar(){
	
}

Car::~Car()
{
}

