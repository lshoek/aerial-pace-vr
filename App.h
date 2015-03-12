#pragma once

#include <VrLib\Application.h>
#include <VrLib\Device.h>
#include <VrLib\Kernel.h>
#include <CaveLib\CaveLib.h>
#include <GL\glew.h>
#include "WiiMoteWrapper.h"
#include <ctime>
#include "Camera.h"

class cTexture;

class App : public Application
{
	private:
		PositionalDevice wand;
		PositionalDevice head;
		DigitalDevice leftButton;
		cTexture* brickwall_texture;
		cModel* checkers_model;
		WiiMoteWrapper * wiiMoteWrapper;
		Camera* camera;
		GLint fps;
		clock_t clock_start = clock();

		int updateCarSpeed(GLfloat timeFactor);

	public:
		App(WiiMoteWrapper * w);
		~App(void);
		virtual void init();
		virtual void preFrame(double, double);
		virtual void draw(const glm::mat4 &projectionMatrix, const glm::mat4 &modelViewMatrix);
		void DrawBrickWall();
			
};