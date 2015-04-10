#pragma comment(lib,"opengl32")
#define _USE_MATH_DEFINES
#include <GL\glew.h>
#include <Windows.h>
#include <CaveLib\texture.h>
#include <CaveLib\model.h>
#include "App.h"
#include <glm/gtc/type_ptr.hpp>
#include <glm\ext.hpp>
#include <functional>

App::App(WiiMoteWrapper* w)
{
	wiiMoteWrapper = w;
}

App::~App(void)
{
	glUseProgram(0);
	if (FBO_ENABLED)
	{
		glDeleteTextures(1, &fbo.fboTextureID);
		glDeleteTextures(1, &fbo.rboTextureID);
		glDeleteFramebuffers(1, &fbo.fboID);
		glDeleteRenderbuffers(1, &fbo.rboID);
	}
}

void App::init(void)
{
	screenSize = glm::vec2(Kernel::getInstance()->getWindowWidth(), Kernel::getInstance()->getWindowHeight());
	m_pDebugDrawer = new DebugDrawer();
	m_pDebugDrawer->setDebugMode(3);
	headDevice.init("MainUserHead");
	cameraDevice.init("CameraPosition");
	physics.bullet3Init();
	cube_model = CaveLib::loadModel("data/aerial-pace-vr/models/cube.obj", new ModelLoadOptions(300.0f));
	checkers_model = CaveLib::loadModel("data/aerial-pace-vr/models/checkers_sphere.obj", new ModelLoadOptions(10.0f));
	sun_model = CaveLib::loadModel("data/aerial-pace-vr/models/checkers_sphere.obj", new ModelLoadOptions(10.0f));
	racetrack_model = CaveLib::loadModel("data/aerial-pace-vr/models/racetrack.obj", new ModelLoadOptions(100.0f));
	normals_texture = CaveLib::loadTexture("data/aerial-pace-vr/textures/normalmap3a.png", new TextureLoadOptions(GL_FALSE));
	pointLight.position = glm::vec3(60.0f, 20.0f, -10.0f);
	pointLight.intensities = glm::vec3(1.0f, 1.0f, 1.0f);
	pointLight.ambientCoefficient = 0.6f;
	pointLight.attentuation = 0.2f;
	physics.addFloor(racetrack_model);

	simpleShader = new ShaderProgram("data/aerial-pace-vr/shaders/simple.vert", "data/aerial-pace-vr/shaders/simple.frag");
	simpleShader->link();

	noiseShader = new ShaderProgram("data/aerial-pace-vr/shaders/perlinnoise.vert", "data/aerial-pace-vr/shaders/perlinnoise.frag");
	noiseShader->link();

	sunShader = new ShaderProgram("data/aerial-pace-vr/shaders/sunshader.vert", "data/aerial-pace-vr/shaders/sunshader.frag");
	sunShader->link();

	airnoiseShader = new ShaderProgram("data/aerial-pace-vr/shaders/airnoise.vert", "data/aerial-pace-vr/shaders/airnoise.frag");
	airnoiseShader->link();

	std::vector<string> shaders{ "normalpostprocess", "blur", "threshold", "mirror1", "mirror2", "shining" };
	for each (string shader in shaders)
		fbo.fboShaders.push_back(new ShaderProgram("data/aerial-pace-vr/shaders/fbo/" + shader + ".vert", "data/aerial-pace-vr/shaders/fbo/" + shader + ".frag"));
	for each (ShaderProgram* program in fbo.fboShaders)
	{
		program->link();
		program->setUniformInt("s_texture", 0);
	}

	physics.world->setDebugDrawer(m_pDebugDrawer);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);

	if (FBO_ENABLED)
	{
		glGenTextures(1, &fbo.fboID);
		glBindTexture(GL_TEXTURE_2D, fbo.fboTextureID);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
		glGenFramebuffers(1, &fbo.fboID);
		glBindFramebuffer(GL_FRAMEBUFFER, fbo.fboID);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, screenSize.x, screenSize.y, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, fbo.fboTextureID, 0);
		glGenRenderbuffers(1, &fbo.rboID);
		glGenTextures(1, &fbo.rboTextureID);
		glBindTexture(1, fbo.rboTextureID);
		glBindRenderbuffer(GL_RENDERBUFFER, fbo.rboID);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, screenSize.x, screenSize.y);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, fbo.rboID);
		//auto b = glewGetErrorString(glGetError());
		GLenum e = glCheckFramebufferStatus(GL_DRAW_FRAMEBUFFER);
		if (e != GL_FRAMEBUFFER_COMPLETE)
			printf("There is a problem with the FBO\n");
	}
}

void App::preFrame(double frameTime, double totalTime)
{
	// timer
	clock_t clock_end = clock();
	GLfloat timeFctr = GLfloat(clock_end - clock_start) / CLOCKS_PER_SEC; // calculate time(s) elapsed since last frame
	fps = int(1 / timeFctr);
	clock_start = clock();
	if (GetAsyncKeyState(82) != 0){
		//physics.addCar();
	}

	if (GetAsyncKeyState(VK_RIGHT)){
		if (nextTimeToCheck < clock()){
			fbo.currentShader++;
			fbo.currentShader %= fbo.fboShaders.size();
			nextTimeToCheck = clock() + 130;
		}
	}

	if (GetAsyncKeyState(VK_LEFT)){
		if (nextTimeToCheck < clock()){
			fbo.currentShader--;
			if (fbo.currentShader == -1)
				fbo.currentShader = fbo.fboShaders.size() - 1;
			nextTimeToCheck = clock() + 130;
		}
	}

	//Check I
	if (GetAsyncKeyState(73) != 0)
	{
		//wiiMoteWrapper->buttonOne = true;
	}
	else
		//wiiMoteWrapper->buttonOne = false;
	//Check J
	if (GetAsyncKeyState(74) != 0)
	{
		//wiiMoteWrapper->degrees = -30;
	}
	//Check K
	if (GetAsyncKeyState(75) != 0)
	{
		//wiiMoteWrapper->buttonTwo = true;
	}
	else
		//wiiMoteWrapper->buttonTwo = false;
	//Check L
	if (GetAsyncKeyState(76) != 0)
	{
		//wiiMoteWrapper->degrees = 30;
	}
	if (GetAsyncKeyState(74) == 0 && GetAsyncKeyState(76) == 0)
		//wiiMoteWrapper->degrees = 0;
	physics.updateCar(timeFctr,wiiMoteWrapper);
}

void App::draw(const glm::mat4 &projectionMatrix, const glm::mat4 &modelViewMatrix)
{
	if (FBO_ENABLED)
	{
		glBindFramebuffer(GL_FRAMEBUFFER, fbo.fboID);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}

	glEnable(GL_TEXTURE_2D);
	glEnable(GL_COLOR_MATERIAL);

	float mvpRaw[16];
	physics.realCar->getWorldTransform().getOpenGLMatrix(mvpRaw);

	// Update the uniform time variable.
	GLfloat time = GLfloat(clock()) / GLfloat(CLOCKS_PER_SEC);

	// Car
	btVector3 carPosition = physics.realCar->getWorldTransform().getOrigin();
	glm::vec3 glmCarPosition(-carPosition.x(), -carPosition.y() - 2, -carPosition.z());

	// Devices
	glm::mat4 headData = headDevice.getData();
	glm::mat4 viewMatrix = modelViewMatrix * headData;

	// Mvp
	glm::mat4 mvp = projectionMatrix * viewMatrix; // glm::mat4 mvp = projectionMatrix * modelViewMatrix;
	mvp = glm::rotate(mvp, -physics.realCar->getWorldTransform().getRotation().getAngle(), glm::vec3(0, 1, 0));
	mvp = glm::translate(mvp, glmCarPosition);

	// Sun
	float scale = 0.3f;
	glm::mat4 sunMat4 = mvp;
	sunMat4 = glm::translate(sunMat4, pointLight.position);
	sunMat4 = glm::scale(sunMat4, glm::vec3(scale, scale, scale));
	
	// Cube Model (Air)
	airnoiseShader->use();
	airnoiseShader->setUniformFloat("time", time);
	airnoiseShader->setUniformMatrix4("modelViewProjectionMatrix", glm::translate(mvp, glm::vec3(-150.0f, -100.0f, 150.0f)));
	cube_model->draw(airnoiseShader);

	// Checkers Model
	simpleShader->use();
	simpleShader->setUniformInt("s_texture", 0);
	glUniform1i(simpleShader->getUniformLocation("s_normals"), 1);
	glBindTexture(GL_TEXTURE_2D, normals_texture->tid());
	simpleShader->setUniformMatrix4("modelViewMatrix", modelViewMatrix);
	simpleShader->setUniformVec3("light.position", pointLight.position);
	simpleShader->setUniformVec3("light.intensities", pointLight.intensities);
	simpleShader->setUniformFloat("light.attenuation", pointLight.attentuation);
	simpleShader->setUniformFloat("light.ambientCoefficient", pointLight.ambientCoefficient);
	simpleShader->setUniformFloat("time", time);
	simpleShader->setUniformVec3("materialSpecularColor", glm::vec3(1.0f, 1.0f, 1.0f));
	simpleShader->setUniformFloat("materialShininess", 5.0f);
	simpleShader->setUniformVec3("cameraPosition", glmCarPosition);
	simpleShader->setUniformMatrix4("modelViewProjectionMatrix", mvp);
	checkers_model->draw(simpleShader);

	// Sun
	sunShader->use();
	sunShader->setUniformMatrix4("modelViewMatrix", modelViewMatrix);
	sunShader->setUniformFloat("time", time);
	sunShader->setUniformMatrix4("modelViewProjectionMatrix", sunMat4);
	sunShader->setUniformVec2("screenSize", screenSize);
	sunShader->setUniformVec3("lightPosition", pointLight.position);
	sun_model->draw(sunShader);

	// Racetrack
	noiseShader->use();
	glUniform1i(noiseShader->getUniformLocation("s_normals"), 0);
	glBindTexture(GL_TEXTURE_2D, normals_texture->tid());
	noiseShader->setUniformMatrix4("modelViewMatrix", modelViewMatrix);
	noiseShader->setUniformVec3("light.position", pointLight.position);
	noiseShader->setUniformVec3("light.intensities", pointLight.intensities);
	noiseShader->setUniformFloat("light.attenuation", pointLight.attentuation);
	noiseShader->setUniformFloat("light.ambientCoefficient", pointLight.ambientCoefficient);
	noiseShader->setUniformFloat("time", time);
	noiseShader->setUniformVec3("materialSpecularColor", glm::vec3(1.0f, 1.0f, 1.0f));
	noiseShader->setUniformVec3("cameraPosition", glmCarPosition);
	noiseShader->setUniformFloat("materialShininess", 5.0f);
	noiseShader->setUniformMatrix4("modelViewProjectionMatrix", mvp);
	racetrack_model->draw(noiseShader);
	physics.world->debugDrawWorld();

	if (FBO_ENABLED)
	{
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		glViewport(0, 0, screenSize.x, screenSize.y);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		std::vector<glm::vec2> verts;
		verts.push_back(glm::vec2(-1, -1));
		verts.push_back(glm::vec2(1, -1));
		verts.push_back(glm::vec2(1, 1));
		verts.push_back(glm::vec2(-1, 1));

		//glBindBuffer(GL_ARRAY_BUFFER, 0);
		ShaderProgram* curFBOShader = fbo.fboShaders[fbo.currentShader];
		curFBOShader->use();
		curFBOShader->setUniformFloat("time", time);
		curFBOShader->setUniformVec2("screenSize", screenSize);
		curFBOShader->setUniformVec3("lightPosition", pointLight.position);
		curFBOShader->setUniformMatrix4("mvp", modelViewMatrix);
		//curFBOShader->setUniformInt("s_texture", 0);
		glBindVertexArray(0);
		glEnableVertexAttribArray(0);							// en vertex attribute 1
		glDisableVertexAttribArray(1);							// disable vertex attribute 1
		glDisableVertexAttribArray(2);							// disable vertex attribute 1
		glBindTexture(GL_TEXTURE_2D, fbo.fboTextureID);
		glVertexAttribPointer(0, 2, GL_FLOAT, false, 2 * 4, &verts[0]);									//geef aan dat de posities op deze locatie zitten
		glDrawArrays(GL_QUADS, 0, verts.size());
		glBindTexture(GL_TEXTURE_2D, 0);
		glDisableVertexAttribArray(0);
	}
	glUseProgram(0);
}

void App::setFboEnabled(bool b) { if (b) FBO_ENABLED = false; }

glm::vec3 App::extractCameraPosition(const glm::mat4 &modelView) // No Scaling
{
	glm::mat3 rotMat = glm::mat3(modelView);
	glm::vec3 d(modelView[3]);
	return -d * rotMat;
}
