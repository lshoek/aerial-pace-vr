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
#include "Physics.h"
#include "DebugFunctions.h"
#include "frameBufferObject.h"


namespace blib { class Texture; class Font; class StaticModel; }

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
		ShaderProgram *simpleShader, *noiseShader, *sunShader;
		//FrameBufferObject fbo;

		void drawStage(const glm::mat4 &mvp1, const btVector3 &translation, float rotation, GLfloat time);

	public:
		App(WiiMoteWrapper * w);
		~App(void);
		
		void init();
		void preFrame(double frameTime, double totalTime);
		void draw(const glm::mat4 &projectionMatrix, const glm::mat4 &modelViewMatrix);
};

class DebugDraw : public btIDebugDraw
{
	std::vector<blib::VertexP3C4> vertices;
	int debugMode;
public:
	blib::RenderState renderstate;
	enum class Uniforms
	{
		ProjectionMatrix,
		CameraMatrix,
	};
	DebugDraw(blib::ResourceManager* resourceManager);
	virtual void drawLine(const btVector3& from, const btVector3& to, const btVector3& color);
	virtual void drawContactPoint(const btVector3& PointOnB, const btVector3& normalOnB, btScalar distance, int lifeTime, const btVector3& color) { }
	void flush(blib::Renderer* renderer, const glm::mat4 &projectionMatrix, const glm::mat4 &cameraMatrix);
	virtual void reportErrorWarning(const char* warningString) { printf("%s\n", warningString); }
	virtual void draw3dText(const btVector3& location, const char* textString) { }
	virtual void setDebugMode(int debugMode)
	{
		this->debugMode = debugMode;
	}
	virtual int getDebugMode() const
	{
		return debugMode;
	}
};