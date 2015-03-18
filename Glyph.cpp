#include <string>
#include <sstream>
#include "Glyph.h"

using namespace std;

Glyph::Glyph()
{
}

Glyph::~Glyph()
{
}

string Glyph::tostring()
{
	stringstream ss;
	ss << "id=" << id << " x=" << x << " y=" << y << " width=" << width << " height=" << height << " yoffset=" << yoffset << " xadvance=" << xadvance;
	return ss.str();
}
