#include "Camera.h"
#include <stdio.h>
#include <math.h>
#include <string>
#include <iostream>
#include <sstream>
#define PI 3.1415926 //3.14159265358979323846

Camera::Camera()
{
	m_yaw = 3.0;
	m_pitch = 0.0;
	setCamPosition(0, 0, 0);
}

void Camera::refresh()
{
	//lookat vectors	
	m_lx = cos(m_yaw) * cos(m_pitch);
	m_ly = sin(m_pitch);
	m_lz = sin(m_yaw) * cos(m_pitch);

	//position vectors
	GLfloat lx = cos(m_yaw)*cos(m_pitch);
	GLfloat ly = sin(m_pitch);
	GLfloat lz = sin(m_yaw)*cos(m_pitch);
	m_x += speed*tf*lx;
	m_y += speed*tf*ly;
	m_z += speed*tf*lz;

	//speed
	if (speed > 0.001 && !isMoving)
		speed *= DECELERATION_SPEED;

	if (prevSpeed > speed && speed < 0.1)
		speed = 0;
	else if (speed >= MAX_SPEED)
		speed = MAX_SPEED;

	//magic
	gluLookAt(m_x, m_y, m_z, m_x + m_lx, m_y + m_ly, m_z + m_lz, 0.0, 1.0, 0.0);

	//rotation dec
	if (rightRotationSpeed > ROTATION_ACCELERATION_SPEED - 0.001) rightRotationSpeed *= DECELERATION_SPEED;
	else rightRotationSpeed = 0;
	if (leftRotationSpeed > ROTATION_ACCELERATION_SPEED - 0.001) leftRotationSpeed *= DECELERATION_SPEED;
	else leftRotationSpeed = 0;

	//rotation acc
	if (rightRotationSpeed > leftRotationSpeed)
		rotationSpeed = rightRotationSpeed - leftRotationSpeed;
	else
		rotationSpeed = (leftRotationSpeed - rightRotationSpeed)*-1;

	//rotation update
	m_yaw += rotationSpeed*(speed / MAX_SPEED)*tf;

	//set movement back to false
	isMoving = false;
}

void Camera::setCamPosition(GLfloat x, GLfloat y, GLfloat z)
{
	m_x = x;
	m_y = y;
	m_z = z;
}

void Camera::getCamPosition(GLfloat &x, GLfloat &y, GLfloat &z)
{
	x = m_x;
	y = m_y;
	z = m_z;
}

void Camera::getDirectionVector(GLfloat &x, GLfloat &y, GLfloat &z)
{
	x = m_lx;
	y = m_ly;
	z = m_lz;
}

void Camera::getPlayerPosition(GLfloat &x, GLfloat &y, GLfloat &z)
{
	GLfloat px, py, pz;
	GLfloat dx, dy, dz;
	getCamPosition(px, py, pz);
	getCamPosition(dx, dy, dz);

	//Normalize direction vector
	GLfloat pLen = sqrt(dx*dx + dy*dy + dz*dz);
	dx /= pLen;
	dy /= pLen;
	dz /= pLen;
	x = px + dx * 10;
	y = 0;
	z = pz + dz * 10;
}

void Camera::updateSpeed()
{
	isMoving = true;
	if (speed < MAX_SPEED)
		speed += ACCELERATION_SPEED*(1 - speed / MAX_SPEED);
}

void Camera::fly(GLfloat incr)
{
	m_y += incr;
}

void Camera::rotateCamYaw(GLfloat angle)
{
	if (angle > 0 && rightRotationSpeed < MAX_ROTATION_SPEED)
		rightRotationSpeed += ROTATION_ACCELERATION_SPEED;
	if (angle < 0 && leftRotationSpeed < MAX_ROTATION_SPEED)
		leftRotationSpeed += ROTATION_ACCELERATION_SPEED;
}

void Camera::rotateCamPitch(GLfloat angle)
{
	const GLfloat limit = 89.0 * PI / 180.0;
	m_pitch += angle;

	if (m_pitch < -limit)
		m_pitch = -limit;

	if (m_pitch > limit)
		m_pitch = limit;
}

std::string Camera::toString()
{
	std::stringstream stream;
	GLfloat px, py, pz;
	getPlayerPosition(px, py, pz);
	stream << "Speed:" << (int)speed << " pX:" << px << " pZ:" << pz << std::endl;
	return stream.str();
}

Camera::~Camera() {}
void Camera::setCamYaw(GLfloat angle) { m_yaw = angle; }
void Camera::setCamPitch(GLfloat angle) { m_pitch = angle; }