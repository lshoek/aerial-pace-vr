#pragma once
#include <GL\glew.h>

inline void DrawWireFrame(void)
{
	glPushMatrix();
	glLineWidth(1.0);
	glColor3f(1.0, 0.0, 0.0);
	glScalef(100.0f, 100.0f, 100.0f);
	glTranslatef(0.0f, -0.05f, 0.0f);
	for (float i = -2.0f; i <= 2.0f; i += 0.1f)
	{
		glBegin(GL_LINES);
		glVertex3f(i, 0.0f, -2.0f);
		glVertex3f(i, 0.0f, 2.0f);
		glEnd();
		glBegin(GL_LINES);
		glVertex3f(-2.0f, 0.0f, i);
		glVertex3f(2.0f, 0.0f, i);
		glEnd();
	}
	glPopMatrix();
}

inline void DrawPoint(glm::vec3 pos)
{
	glPushMatrix();
	glPointSize(10.0);
	glColor3f(0.0f, 1.0f, 0.0f);
	glBegin(GL_POINTS);
	glVertex3f(pos.x, pos.y, pos.z);
	glEnd();
	glPopMatrix();
}

inline void DrawAxii()
{
	glPushMatrix();
	glLineWidth(3.0f);
	for (float i = -2.0f; i <= 2.0f; i += 0.1f)
	{
		glBegin(GL_LINES);
		glColor3f(1.0f, 0.0f, 0.0f);
		glVertex3f(0.0f, 0.0f, 0.0f);
		glVertex3f(1.0f, 0.0f, 0.0f);
		glEnd();
		glBegin(GL_LINES);
		glColor3f(0.0f, 1.0f, 0.0f);
		glVertex3f(0.0f, 0.0f,0.0f);
		glVertex3f(0.0f, 1.0f, 0.0f);
		glEnd();
		glBegin(GL_LINES);
		glColor3f(0.0f, 0.0f, 1.0f);
		glVertex3f(0.0f, 0.0f, 0.0f);
		glVertex3f(0.0f, 0.0f, 1.0f);
		glEnd();
	}
	glPopMatrix();
}