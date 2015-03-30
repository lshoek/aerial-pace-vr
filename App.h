#pragma once

#include <VrLib\Application.h>
#include <VrLib\Device.h>
#include <VrLib\Kernel.h>
#include <CaveLib\CaveLib.h>
#include <CaveLib\Shader.h>
#include <GL\glew.h>
#include <glm/gtc/matrix_transform.hpp>
#include <ctime>
#include <iostream>

#include "WiiYourself/WiiMoteWrapper.h"
#include "Camera.h"
#include "Physics.h"
#include "DebugFunctions.h"

class App : public Application
{
	private:	
		struct Light
		{
			glm::vec3 position;
			glm::vec3 intensities; //a.k.a. the color of the light
		};
		//PositionalDevice wand;
		//PositionalDevice head;
		DigitalDevice leftButton;
		DigitalDevice upArrow, downArrow, leftArrow, rightArrow;
		cModel* cube_model,* checkers_model,* racetrack_model;
		WiiMoteWrapper * wiiMoteWrapper;
		Camera* camera;
		GLint fps;
		clock_t clock_start = clock();
		Physics physics;
		ShaderProgram *simpleShader, *noiseShader, *airnoiseShader;
		Light pointLight;

	public:
		App(WiiMoteWrapper * w);
		~App(void);

		static const string SHADERLOCATION;

		void init();
		void preFrame(double frameTime, double totalTime);
		void draw(const glm::mat4 &projectionMatrix, const glm::mat4 &modelViewMatrix);
};