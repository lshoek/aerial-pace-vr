#include <cstdlib>
#include "App.h"
#include <VrLib/Kernel.h>
#include "WiiMoteWrapper.h"
#include <thread>

using namespace std;

int main(int argc, char* argv[])
{
	Kernel* kernel = Kernel::getInstance();  // Get the kernel
	WiiMoteWrapper w;
	App* app = new App(&w);             // Instantiate an instance of the app	
	// Load any config files specified on the command line	
	
	thread t([&](WiiMoteWrapper * w2){ w2->start(); },&w);
	while (w.status == 0){}
	if (w.status != 1)
	{
		printf("geen wiimote");
		std::string name;
		std::getline(std::cin, name);
		return -1;
	}
	for (int i = 1; i < argc; ++i)
	{
		if (strcmp(argv[i], "--config") == 0)
		{
			i++;
			kernel->loadConfig(argv[i]);
		}
	}
	kernel->setApp(app);
	kernel->start();	
	delete app;
	//stop wii library
	w.continueGame = false;
	t.join();
	return 0;
}

