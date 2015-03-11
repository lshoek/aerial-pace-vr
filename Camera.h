#pragma once
#include <GL\glew.h>

class Camera
{
	public:
		Camera();
		~Camera();
		void SetTimeFctr(float);
		GLfloat tf;
};