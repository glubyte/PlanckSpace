#include "fps.h"

void initFPS()
{
	fpsshit.frames = 0;
	fpsshit.sdltimer = 0;
}
void calculateFPS() 
{
	//fpsshit->dt = difftime(time(NULL), fpsshit->timer);
	fpsshit.dt = SDL_GetTicks() - fpsshit.sdltimer;

	if (fpsshit.dt > 1000)
	{
		fpsshit.fps = 1000 * fpsshit.frames / fpsshit.dt;
		fpsshit.frames = 0;
		fpsshit.sdltimer = SDL_GetTicks();

		printf("FPS: %i\n", fpsshit.fps);
	}
}