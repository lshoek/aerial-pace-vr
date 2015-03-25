#pragma once

#include <VrLib\Application.h>
#include <VrLib\Device.h>
#include <VrLib\Kernel.h>
#include <CaveLib\CaveLib.h>
#include <CaveLib\Shader.h>
#include <GL\glew.h>
#include <glm/gtc/matrix_transform.hpp>

#include "WiiYourself/WiiMoteWrapper.h"
#include <ctime>
#include "Camera.h"
#include "Car.h"
#include "Physics.h"
#include "DebugFunctions.h"

class App : public Application
{
	private:
		//PositionalDevice wand;
		//PositionalDevice head;
		DigitalDevice leftButton;
		DigitalDevice upArrow, downArrow, leftArrow, rightArrow;
		cModel* checkers_model,* racetrack_model;
		WiiMoteWrapper * wiiMoteWrapper;
		Camera* camera;
		GLint fps;
		clock_t clock_start = clock();
		Physics physics;
		ShaderProgram *simpleShader, *noiseShader;

	public:
		App(WiiMoteWrapper * w);
		~App(void);
		Car car;

		void init();
		void preFrame(double frameTime, double totalTime);
		void draw(const glm::mat4 &projectionMatrix, const glm::mat4 &modelViewMatrix);
};