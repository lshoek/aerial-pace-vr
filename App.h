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
#include "Physics.h"
#include "DebugFunctions.h"
#include "DebugDrawer.h"


namespace blib { class Texture; class Font; class StaticModel; }

class App : public Application
{
	private:	
		struct Light
		{
			glm::vec3 position;
			glm::vec3 intensities; //a.k.a. the color of the light
			float ambientCoefficient;
			float attentuation;
		};
		PositionalDevice headDevice, cameraDevice;
		cTexture* normals_texture,* normals_texture2;
		cModel* cube_model,* checkers_model,* racetrack_model, * sun_model;
		WiiMoteWrapper * wiiMoteWrapper;
		DebugDrawer* m_pDebugDrawer;
		GLint fps;
		clock_t clock_start = clock();
		Physics physics;
		ShaderProgram *simpleShader, *noiseShader, *sunShader, *airnoiseShader;
		Light pointLight;
		

	public:
		glm::vec2 screenSize;
		App(WiiMoteWrapper * w);
		~App(void);
		
		void init();
		void preFrame(double frameTime, double totalTime);
		void draw(const glm::mat4 &projectionMatrix, const glm::mat4 &modelViewMatrix);

		glm::vec3 extractCameraPosition(const glm::mat4 &modelView);
		GLuint fboID;
		GLuint rboID;
		GLuint fboTextureID;
		ShaderProgram *fboShader;
};