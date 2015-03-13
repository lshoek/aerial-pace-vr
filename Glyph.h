#pragma once
#include <string>
#include <sstream>

using namespace std;

class Glyph
{
public:
	char id;
	int x, y, width, height, xoffset, yoffset, xadvance;
	Glyph();
	~Glyph();
	string tostring();
};

