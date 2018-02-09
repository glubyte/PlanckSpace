#include "init.h"

unsigned char init()
{
	// SDL
	if (initSDL())
	{
		printf("SDL failed to initialize.\n");
		getchar();
		return 1;
	}
	printf("SDL successfully initialized.\n");

	// Vulkan
	if (initVk())
	{
		printf("Vulkan failed to initialized.\n");
		getchar();
		return 1;
	}
	printf("Vulkan succcessfully initialized.\n");

	// Local
	initFPS();
	initProj();

	return 0;
}
void flushInput()
{
	while (getchar() != '\n')
		continue;
}