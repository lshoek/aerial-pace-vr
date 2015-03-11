#include <cstdlib>
#include "App.h"
#include <VrLib/Kernel.h>
#include "WiiMoteWrapper.h"
#include <thread>

using namespace std;



int main(int argc, char* argv[])
{
	Kernel* kernel = Kernel::getInstance();  // Get the kernel
	App* app = new App();             // Instantiate an instance of the app	
	// Load any config files specified on the command line	
	WiiMoteWrapper w;
	thread t([&](WiiMoteWrapper * w2){ w2->start(); },&w);

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
	w.continueGame = false;
	t.join();
	return 0;
}

