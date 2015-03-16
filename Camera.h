#pragma comment(lib, "glu32.lib")
#include <GL\glew.h>
#include <stdio.h>
#include <math.h>
#include <string>
#include <sstream>

class Camera
{
	public:
		Camera();
		~Camera();
		GLfloat tf;

		void refresh();
		void updateSpeed();
		void fly(GLfloat incr);
		void rotateCamYaw(GLfloat angle);
		void rotateCamPitch(GLfloat angle);

		void getPlayerPosition(GLfloat &, GLfloat &, GLfloat &);
		void getDirectionVector(GLfloat &x, GLfloat &y, GLfloat &z);
		void setCamPosition(GLfloat x, GLfloat y, GLfloat z);
		void getCamPosition(GLfloat &x, GLfloat &y, GLfloat &z);
		void setCamYaw(GLfloat angle);
		void setCamPitch(GLfloat angle);
		std::string toString();

		GLfloat m_yaw, m_pitch;
		GLfloat rightRotationSpeed = 0;
		GLfloat leftRotationSpeed = 0;
		GLfloat rotationSpeed = 0;

	private:
		bool isMoving = false;
		GLfloat speed = 0;
		GLfloat prevSpeed = 0;
		const GLfloat MAX_SPEED = 50.0;						// DEFAULT 50.0
		const GLfloat MAX_ROTATION_SPEED = 2.5;				// DEFAULT 2.5
		const GLfloat ACCELERATION_SPEED = 1.0;				// DEFAULT 0.1
		const GLfloat DECELERATION_SPEED = 0.99;			// DEFAULT 0.99
		const GLfloat ROTATION_ACCELERATION_SPEED = 0.04;	// DEFAULT 0.04

		GLfloat m_x, m_y, m_z;
		GLfloat m_lx, m_ly, m_lz;
};