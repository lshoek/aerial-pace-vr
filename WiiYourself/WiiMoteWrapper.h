#pragma once
#include "WiiLib.h"
#include "wiimote.h"
class WiiMoteWrapper
{
public:
	bool continueGame;	
	void start();
	float degrees;
	bool buttonOne;
	bool buttonTwo;
	bool buttonHome;
	int status;

	WiiMoteWrapper();
	~WiiMoteWrapper();
private:
	int wiiMoteMainStart();
};

