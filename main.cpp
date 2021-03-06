#include <cstdlib>
#include "App.h"
#include <VrLib/Kernel.h>
#include "WiiYourself/WiiMoteWrapper.h"
#include <thread>

using namespace std;

int main(int argc, char* argv[])
{
	Kernel* kernel = Kernel::getInstance();  // Get the kernel
	WiiMoteWrapper w;
	App* app = new App(&w);             // Instantiate an instance of the app	
	// Load any config files specified on the command line		
	thread t([&](WiiMoteWrapper * w2){ w2->start(); },&w);
	for (int i = 1; i < argc; ++i)
	{
		if (strcmp(argv[i], "--config") == 0)
		{
			i++;
			kernel->loadConfig(argv[i]);
			if (argv[i] != "oculus.json")
				app->setSimEnabled(true);
		}
	}	
	kernel->setApp(app);
	kernel->start();	
	delete app;
	w.continueGame = false;
	t.join();
	return 0;
}

