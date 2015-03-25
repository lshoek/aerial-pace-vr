#include "Car.h"

Car::Car()
{
	carSpeed = 0;
	carRadians = 0;
	MAXFORCE = 5.0f;
	
	direction = btVector3(1, 0, 0);
	speedVector = btVector3(0, 0, 0);
	position = btVector3(0, 0, 0);
}

void Car::initCar(WiiMoteWrapper* w, Physics* p){
	physics = p;
	wiiMoteWrapper = w;
}

Car::~Car()
{
}

void Car::updateCar(float timeFactor){
	float deltaspeed = 0;
	if (wiiMoteWrapper->buttonOne){//speed goes down
		deltaspeed -= MAXFORCE * timeFactor;
	}
	if (wiiMoteWrapper->buttonTwo){//speed goes up
		deltaspeed += MAXFORCE * timeFactor;
	}
	//deltaspeed += MAXFORCE * timeFactor;
	//carSpeed += deltaspeed;
	btVector3 deltaposition(deltaspeed, 0, 0);
	
	btVector3 torque(deltaspeed, 0, 0);
	torque = torque.rotate(btVector3(0, 1.0f, 0), btRadians(wiiMoteWrapper->degrees ));
	//physics->realCar->applyTorque(torque);
	physics->realCar->applyForce(deltaposition.rotate(btVector3(0, 1.0f, 0), btRadians(wiiMoteWrapper->degrees)), btVector3(0, 0, 0));
	physics->realCar->activate();
	
	//physics->realCar->activate();
	physics->world->stepSimulation(timeFactor);//en updaten	

	btVector3 b2 = physics->realCar->getWorldTransform().getOrigin();
	float x = 0;
	printf("auto %f,%f,%f :%f rad \n", b2.x(), b2.y(), b2.z(), x);

	return;
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
	btVector3 dir = direction;//u
	btVector3 traction;//Ftraction
	btVector3 braking(0,0,0);//Fbraking
	float engineForce = carSpeed;//Engineforce
	btVector3 speed = speedVector;///v
	float cdrag = 0.3f;//Cdrag
	float cbraking = 0.8f;
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
	float r = (btRadians(wiiMoteWrapper->degrees));
	dir = dir.rotate(btVector3(0, 1.0f, 0), r);
	dir = dir.normalize();
	if (wiiMoteWrapper->buttonOne){
		//braking = -dir*cbraking;
	}
	engineForce *= 0.9;
	//traction:Ftraction
	traction = engineForce * dir;
	//drag:weerstandskracht Fdrag
	drag = -cdrag * speed *speed.length();
	//rollResistance:Frr
	rollResistance = -0 * speed;
	//totalForce:Ftotaal
	totalForce = traction + drag + rollResistance + braking;
	//acceleration:a
	acceleration = totalForce / 10.0f;
	//speed:v
	speed += acceleration*timeFactor;
	//het een en ander in car zetten
	speedVector = speed;
	direction = dir;
	carSpeed = engineForce;
	carRadians += r;
	//positie
	position += timeFactor*speed;
	//bullet updaten
	physics->realCar->applyCentralForce(speed);
	physics->realCar->activate();
	physics->world->stepSimulation(timeFactor);//en updaten	
	//debug
	btVector3 b = physics->realCar->getWorldTransform().getOrigin();
	b = position;
	//printf("%f,%f,%f\n", totalForce.x(), totalForce.y(), totalForce.z());
	printf("auto %f,%f,%f :%f rad %f m/s\n", b.x(), b.y(), b.z(), carRadians, engineForce);
}