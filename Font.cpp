#include <string>
#include <iostream>
#include <fstream>
#include <iomanip>
#include <vector>
#include <map>
#include "Font.h"
#include "glyph.h"

using namespace std;

Font::Font(const string fontFile, const string textureFile)
{
	fontTexture = CaveLib::loadTexture(textureFile);
	ifstream input;
	input.open(fontFile);
	while (!input.eof())
	{
		string line;
		getline(input, line);

		line = replace(line, "\t", " "); //tabs become spaces
		line = replace(line, "=", " "); //equals become spaces
		while (line.find("  ") != string::npos)
			line = replace(line, "  ", " "); //double spaces become singles

		if (line == "")
			continue;
		if (line[0] == ' ')
			line = line.substr(1);
		if (line == "")
			continue;
		if (line[line.length() - 1] == ' ')
			line = line.substr(0, line.length() - 1);
		if (line == "")
			continue;

		vector<string> params = split(line, " ");
		Glyph g{};
		g.id = 0;
		int index = 0;
		for (int i = 0; i < params.size(); i++)
		{
			if (params[i] == "id")
				g.id = stoi(params.at(i + 1));
			else if (params[i] == "x")
				g.x = stoi(params[i + 1]);
			else if (params[i] == "y")
				g.y = stoi(params[i + 1]);
			else if (params[i] == "width")
				g.width = stoi(params[i + 1]);
			else if (params[i] == "height")
				g.height = stoi(params[i + 1]);
			else if (params[i] == "yoffset")
				g.yoffset = stoi(params[i + 1]);
			else if (params[i] == "xadvance")
				g.xadvance = stoi(params[i + 1]);
		}
		cgmap it = charmap.begin();
		if (g.id != 0)
			charmap.insert(it, std::pair<char, Glyph>(g.id, g));
	}
	cout << "> " << fontFile << " loaded" << endl;
	//printFontData();

	charList = glGenLists(256);
	for (int i = 0; i < 256; i++)
	{
		if (charmap.find(i) == charmap.end())
			continue;
		Glyph &g = charmap[i];
		float cx = g.x / 128.0f;
		float cy = g.y / 128.0f;
		float sizex = g.width / 128.0f;
		float sizey = g.height / 128.0f;

		glNewList(charList + i, GL_COMPILE);
		glBegin(GL_QUADS);
		glTexCoord2f(cx, cy);					glVertex2d(0, g.height);
		glTexCoord2f(cx, cy + sizey);			glVertex2d(0, 0);
		glTexCoord2f(cx + sizex, cy + sizey);	glVertex2d(g.width, 0);
		glTexCoord2f(cx + sizex, cy);			glVertex2d(g.width, g.height);
		glEnd();
		glTranslatef(g.xadvance, 0, 0);
		glEndList();
	}
}

void Font::drawText(const string text, const GLfloat x, const GLfloat y, const GLfloat scale)
{
	glBindTexture(GL_TEXTURE_2D, fontTexture->tid());
	glEnable(GL_TEXTURE_2D);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glDisable(GL_DEPTH_TEST);
	glOrtho(0, 1280, 0, 720, -1, 100);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glTranslatef(x, y, 0);
	glScalef(scale, scale, 1);
	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
	glListBase(charList);

	glPushMatrix();
	glCallLists(text.length(), GL_UNSIGNED_BYTE, text.c_str());
	glPopMatrix();
}

Font::~Font()
{
}

void Font::printFontData()
{
	for (cgmap iterator = charmap.begin(); iterator != charmap.end(); iterator++)
		cout << "KEY:" << iterator->first << " VALUE: " << iterator->second.tostring() << endl;
}

string Font::replace(string str, string toReplace, string replacement)
{
	size_t index = 0;
	while (true)
	{
		index = str.find(toReplace, index);
		if (index == std::string::npos)
			break;
		str.replace(index, toReplace.length(), replacement);
		++index;
	}
	return str;
}

vector<string> Font::split(string str, string sep)
{
	vector<string> ret;
	size_t index;
	while (true)
	{
		index = str.find(sep);
		if (index == string::npos)
			break;
		ret.push_back(str.substr(0, index));
		str = str.substr(index + 1);
	}
	ret.push_back(str);
	return ret;
}