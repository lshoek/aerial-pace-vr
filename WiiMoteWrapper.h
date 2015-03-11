#pragma once
#include "WiiLib.h"
#include "wiimote.h"
class WiiMoteWrapper
{
public:
	bool continueGame;	
	void start();

	WiiMoteWrapper();
	~WiiMoteWrapper();
private:
	int wiiMoteMainStart();
};

