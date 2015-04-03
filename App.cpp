#pragma comment(lib,"opengl32")
#define _USE_MATH_DEFINES
#include <GL\glew.h>
#include <Windows.h>
#include <CaveLib\model.h>
#include "App.h"
#include <glm/gtc/type_ptr.hpp>
#include <functional>

App::App(WiiMoteWrapper* w)
{
	wiiMoteWrapper = w;
}
App::~App(void){
	glUseProgram(0);
	glDeleteTextures(1, &fboTextureID);
	glDeleteFramebuffers(1, &fboID);
	glDeleteRenderbuffers(1, &rboID);
}


void App::init(void)
{
	m_pDebugDrawer = new DebugDrawer();
	m_pDebugDrawer->setDebugMode(3);
	//upArrow.init("UpArrow"); downArrow.init("DownArrow"); leftArrow.init("LeftArrow"); rightArrow.init("RightArrow");
	headDevice.init("MainUserHead");
	cameraDevice.init("CameraPosition");
	physics.bullet3Init(wiiMoteWrapper);
	cube_model = CaveLib::loadModel("data/aerial-pace-vr/models/cube.obj", new ModelLoadOptions(300.0f));
	checkers_model = CaveLib::loadModel("data/aerial-pace-vr/models/checkers_sphere.obj", new ModelLoadOptions(10.0f));
	sun_model = CaveLib::loadModel("data/aerial-pace-vr/models/checkers_sphere.obj", new ModelLoadOptions(10.0f));
	racetrack_model = CaveLib::loadModel("data/aerial-pace-vr/models/racetrack.obj", new ModelLoadOptions(100.0f));
	camera = new Camera();
	pointLight.position = glm::vec3(-30.0f, 5.0f, 20.0f);
	pointLight.intensities = glm::vec3(1.0f, 1.0f, 1.0f);
	pointLight.ambientCoefficient = 0.5f;
	pointLight.attentuation = 0.2f;
	physics.addFloor(racetrack_model);

	simpleShader = new ShaderProgram("data/aerial-pace-vr/shaders/simple.vert", "data/aerial-pace-vr/shaders/simple.frag");
	simpleShader->link();
	simpleShader->setUniformInt("s_texture", 0);

	noiseShader = new ShaderProgram("data/aerial-pace-vr/shaders/perlinnoise.vert", "data/aerial-pace-vr/shaders/perlinnoise.frag");
	noiseShader->link();
	noiseShader->setUniformInt("s_texture", 0);

	sunShader = new ShaderProgram("data/aerial-pace-vr/shaders/sunshader.vert", "data/aerial-pace-vr/shaders/sunshader.frag");
	sunShader->link();

	airnoiseShader = new ShaderProgram("data/aerial-pace-vr/shaders/airnoise.vert", "data/aerial-pace-vr/shaders/airnoise.frag");
	airnoiseShader->link();
	airnoiseShader->setUniformInt("s_texture", 0);

	fboShader = new ShaderProgram("data/aerial-pace-vr/shaders/postprocess.vert", "data/aerial-pace-vr/shaders/postprocess.frag");
	fboShader->link();
	fboShader->setUniformInt("s_texture", 0);

	physics.world->setDebugDrawer(m_pDebugDrawer);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	//fbo
	if (useFBO == false)
		return;
	glGenTextures(1, &fboID);
	glBindTexture(GL_TEXTURE_2D, fboTextureID);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

	glGenFramebuffers(1, &fboID);
	glBindFramebuffer(GL_FRAMEBUFFER, fboID);


	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1264, 682, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, fboTextureID, 0);

	glGenRenderbuffers(1, &rboID);
	glBindRenderbuffer(GL_RENDERBUFFER, rboID);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, 1264 * 2, 682 * 2);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rboID);
	
	int a = glGetError();
	auto b = glewGetErrorString(a);

	GLenum e = glCheckFramebufferStatus(GL_DRAW_FRAMEBUFFER);

	if (e != GL_FRAMEBUFFER_COMPLETE)
		printf("There is a problem with the FBO\n");
}

void App::preFrame(double frameTime, double totalTime)
{
	// timer
	clock_t clock_end = clock();
	GLfloat timeFctr = GLfloat(clock_end - clock_start) / CLOCKS_PER_SEC; // calculate time(s) elapsed since last frame
	camera->tf = timeFctr;
	fps = int(1 / timeFctr);
	clock_start = clock();
	if (GetAsyncKeyState(82) != 0){
		//physics.addCar();
	}

	//Check I
	if (GetAsyncKeyState(73) != 0)
	{
		wiiMoteWrapper->buttonOne = true;
	}
	else
		wiiMoteWrapper->buttonOne = false;
	//Check J
	if (GetAsyncKeyState(74) != 0)
	{
		wiiMoteWrapper->degrees = -30;
	}
	//Check K
	if (GetAsyncKeyState(75) != 0)
	{
		wiiMoteWrapper->buttonTwo = true;
	}
	else
		wiiMoteWrapper->buttonTwo = false;
	//Check L
	if (GetAsyncKeyState(76) != 0)
	{
		wiiMoteWrapper->degrees = 30;
	}
	if (GetAsyncKeyState(74) == 0 && GetAsyncKeyState(76) == 0)
		wiiMoteWrapper->degrees = 0;
	physics.updateCar(timeFctr);

	//camera
	/*if (upArrow.getData() == ON)
	camera->updateSpeed();
	if (leftArrow.getData() == ON)
	camera->rotateCamYaw(-1.0f);
	else if (rightArrow.getData() == ON)
	camera->rotateCamYaw(1.0f);*/
}

void App::draw(const glm::mat4 &projectionMatrix, const glm::mat4 &modelViewMatrix)
{
	if (useFBO)
		glBindFramebuffer(GL_FRAMEBUFFER, fboID);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	//
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_COLOR_MATERIAL);

	float mvpRaw[16];
	physics.realCar->getWorldTransform().getOpenGLMatrix(mvpRaw);

	// Car
	btVector3 carTranslation = physics.realCar->getWorldTransform().getOrigin();
	glm::vec3 cameraPosition(0, 0, 10.0f);
	glm::vec3 glmCarTranslation(-carTranslation.x(), -carTranslation.y() - 2, -carTranslation.z());

	// Devices
	glm::mat4 headData = headDevice.getData();
	glm::mat4 viewMatrix = modelViewMatrix * headData;

	// Update the uniform time variable.
	GLfloat time = GLfloat(clock()) / GLfloat(CLOCKS_PER_SEC);

	// Mvp
	glm::mat4 mvp = projectionMatrix * viewMatrix; // glm::mat4 mvp = projectionMatrix * modelViewMatrix;
	mvp = glm::rotate(mvp, -physics.realCar->getWorldTransform().getRotation().getAngle(), glm::vec3(0, 1, 0));
	mvp = glm::translate(mvp, glmCarTranslation);


	// Sun
	float scale = 0.1f;
	glm::mat4 sunMat4 = mvp;
	//
	sunMat4 = glm::translate(sunMat4, pointLight.position);
	sunMat4 = glm::scale(sunMat4, glm::vec3(scale, scale, scale));
	//fbo
	
	// Cube Model (Air)
	airnoiseShader->use();
	airnoiseShader->setUniformFloat("time", time);
	airnoiseShader->setUniformMatrix4("modelViewProjectionMatrix", glm::translate(mvp, glm::vec3(-150.0f, -100.0f, 150.0f)));
	cube_model->draw(airnoiseShader);

	// Checkers Model
	simpleShader->use();
	simpleShader->setUniformMatrix4("modelViewMatrix", modelViewMatrix);
	simpleShader->setUniformVec3("light.position", pointLight.position);
	simpleShader->setUniformVec3("light.intensities", pointLight.intensities);
	simpleShader->setUniformFloat("light.attenuation", pointLight.attentuation);
	simpleShader->setUniformFloat("light.ambientCoefficient", pointLight.ambientCoefficient);
	simpleShader->setUniformFloat("time", time);
	simpleShader->setUniformVec3("materialSpecularColor", glm::vec3(1.0f, 1.0f, 1.0f));
	simpleShader->setUniformFloat("materialShininess", 5.0f);
	simpleShader->setUniformVec3("cameraPosition", extractCameraPosition(mvp));
	simpleShader->setUniformMatrix4("modelViewProjectionMatrix", mvp);
	checkers_model->draw(simpleShader);

	sunShader->use();
	sunShader->setUniformMatrix4("modelViewMatrix", modelViewMatrix);
	sunShader->setUniformFloat("time", time);
	sunShader->setUniformMatrix4("modelViewProjectionMatrix", sunMat4);
	sun_model->draw(sunShader);

	// Racetrack
	noiseShader->use();
	noiseShader->setUniformMatrix4("modelViewMatrix", modelViewMatrix);
	noiseShader->setUniformVec3("light.position", pointLight.position);
	noiseShader->setUniformVec3("light.intensities", pointLight.intensities);
	noiseShader->setUniformFloat("light.attenuation", pointLight.attentuation);
	noiseShader->setUniformFloat("light.ambientCoefficient", pointLight.ambientCoefficient);
	noiseShader->setUniformFloat("time", time);
	noiseShader->setUniformVec3("materialSpecularColor", glm::vec3(1.0f, 1.0f, 1.0f));
	noiseShader->setUniformVec3("cameraPosition", extractCameraPosition(mvp));
	noiseShader->setUniformFloat("materialShininess", 5.0f);
	noiseShader->setUniformMatrix4("modelViewProjectionMatrix", mvp);
	racetrack_model->draw(noiseShader);
	physics.world->debugDrawWorld();

	if (useFBO){
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glViewport(0, 0, 1264, 682);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		std::vector<glm::vec2> verts;
		verts.push_back(glm::vec2(-1, -1));
		verts.push_back(glm::vec2(1, -1));
		verts.push_back(glm::vec2(1, 1));
		verts.push_back(glm::vec2(-1, 1));

		//glBindBuffer(GL_ARRAY_BUFFER, 0);
		
		fboShader->use();
		noiseShader->setUniformFloat("time", GLfloat(clock()));
		glBindVertexArray(0);
		glEnableVertexAttribArray(0);							// en vertex attribute 1
		glDisableVertexAttribArray(1);							// disable vertex attribute 1
		glDisableVertexAttribArray(2);							// disable vertex attribute 1


		glBindTexture(GL_TEXTURE_2D, fboTextureID);
		glVertexAttribPointer(0, 2, GL_FLOAT, false, 2 * 4, &verts[0]);									//geef aan dat de posities op deze locatie zitten
		glDrawArrays(GL_QUADS, 0, verts.size());
		glBindTexture(GL_TEXTURE_2D, 0);
		glDisableVertexAttribArray(0);

	}
	glUseProgram(0);
}

// No Scaling
glm::vec3 App::extractCameraPosition(const glm::mat4 &modelView)
{
	glm::mat3 rotMat = glm::mat3(modelView);
	glm::vec3 d(modelView[3]);
	return -d * rotMat;
}
