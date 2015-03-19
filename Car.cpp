#include "Car.h"

Car::Car()
{
	carSpeed = steeringWheelDegrees = 0;
	carRadians = 0;
	MAXFORCE = 10.0f;
	
	direction = btVector3(0, 0, 0);
	
}

void Car::initCar(WiiMoteWrapper* w, Physics* p){
	physics = p;
	wiiMoteWrapper = w;
}

Car::~Car()
{
}

void Car::updateCar(float timeFactor){
	//u = richting waar de auto heengaat
	//Ftraction = u * Engineforce (nieuwe snelheid,engineforce is van knop 1 en 2)

	//Fdrag = -Cdrag * v * | v | (weerstandskracht (vector), v is de vector van de huidige snelheid)

	//rolweerstand
	//Frr = - Crr * v
	//where Crr is a constant and v is the velocity vector.

	//Ftotaal (Flong) = Ftraction + Fdrag + Frr of Ftotaal = Fbraking + Fdrag   + Frr (versnellen of remmen)
	//Fbraking = Fbraking = -u * Cbraking (als het nodig is

	//a = F / M (versnellingsvector = Ftotaal / massa

	//v = v + dt*a (nieuwe snelheid, v = snelheidsvector)

	//de benodigde variabelen
	btVector3 dir;//u
	btVector3 traction;//Ftraction
	float engineForce = carSpeed;//Engineforce
	btVector3 speed = direction;///v
	float cdrag = 0.3f;//Cdrag
	btVector3 drag;//weerstandskracht Fdrag	
	btVector3 rollResistance;//Frr
	btVector3 acceleration;//a
	float mass;//massa
	btVector3 totalForce;//Ftotaal
	//variabelen instellen
	//engineForce:Engineforce
	if (wiiMoteWrapper->buttonOne){//speed goes down
		engineForce -= MAXFORCE * timeFactor;
	}
	if (wiiMoteWrapper->buttonTwo){//speed goes up
		engineForce += MAXFORCE * timeFactor;
	}
	btClamp<float>(engineForce, -MAXFORCE, MAXFORCE);
	//dir:u
	float r = (btRadians(wiiMoteWrapper->degrees) + carRadians);
	//traction:Ftraction
	traction = btVector3(engineForce, 0, 0).rotate(btVector3(0, 1.0f, 0), r);
	//drag:weerstandskracht Fdrag
	drag = -cdrag * speed *speed.length();
	//rollResistance:Frr
	rollResistance = -0 * speed;
	//totalForce:Ftotaal
	totalForce = traction + drag + rollResistance;
	//acceleration:a
	acceleration = totalForce / 10.0f;
	//speed:v
	speed += acceleration*timeFactor;
	//het een en ander in car zetten
	direction = speed;
	carSpeed = engineForce;
	carRadians = r;
	//bullet updaten
	physics->realCar->applyCentralForce(speed);
	physics->realCar->activate();
	physics->world->stepSimulation(timeFactor);//en updaten	
	//debug
	btVector3 b = physics->realCar->getWorldTransform().getOrigin();
	b = direction;
	//printf("%f,%f,%f\n", totalForce.x(), totalForce.y(), totalForce.z());
	printf("auto %f,%f,%f :%f rad %f m/s\n", b.x(), b.y(), b.z(), carRadians, b.length());
}