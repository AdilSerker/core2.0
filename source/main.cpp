#include "App.h"

App *app = nullptr;

int main()
{
	app = new App();

	app->init();
	app->run();

	delete app;

	return 0;
}
