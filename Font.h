#pragma once
#include <GL/glew.h>
#include <string>
#include <iostream>
#include <fstream>
#include <iomanip>
#include <vector>
#include <map>
#include "glyph.h"
#include <CaveLib\texture.h>

using namespace std;

class Font
{
	public:
		Font(const string, const string);
		~Font();
		void drawText(string, const GLfloat, const GLfloat, const GLfloat);

	private:
		GLint charList;
		cTexture* fontTexture;
		map<char, Glyph> charmap;
		typedef map<char, Glyph>::iterator cgmap;
		string replace(string, string, string);
		vector<string> split(string, string);
		void printFontData();
};


