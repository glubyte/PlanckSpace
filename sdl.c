#include "sdl.h"

unsigned char initSDL()
{
	sdlshit.winWidth = 800; // ultimately from config file
	sdlshit.winHeight = 600; // ultimately from config file
	SDL_VERSION(&sdlshit.windowInfo.version);
	sdlshit.eventTimer = 0;

	if (SDL_Init(SDL_INIT_VIDEO))
	{
		printf("SDL video module failed to initialize: %s\n", SDL_GetError());
		return 1;
	}

	printf("Creating window...\n");
	if (createWindow())
		return 1;

	// text
	if (TTF_Init() == -1)
	{
		printf("Failed to initialize SDL TTF.\n");
		return 1;
	}

	sdlshit.font = TTF_OpenFont("font.ttf", 16);
	//TTF_SetFontHinting(sdlshit.font, TTF_HINTING_MONO);

	return 0;
}
unsigned char createWindow()
{
	SDL_DisplayMode mode;
	// create window
	sdlshit.window = SDL_CreateWindow("Planck Space", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, sdlshit.winWidth, sdlshit.winHeight, SDL_WINDOW_RESIZABLE);

	if (sdlshit.window == NULL)
	{
		printf("Window could not be created: %s\n", SDL_GetError());
		return 1;
	}

	// acquire desktop display information and apply for fullscreen
	SDL_GetDesktopDisplayMode(0, &mode);
	SDL_SetWindowDisplayMode(sdlshit.window, &mode);

	// acquire window manager information
	if (SDL_GetWindowWMInfo(sdlshit.window, &sdlshit.windowInfo))
	{
		switch (sdlshit.windowInfo.subsystem)
		{
		case SDL_SYSWM_WINDOWS:
			printf("Win32 window manager detected...\n");
			return 0;
		case SDL_SYSWM_X11:
			printf("X11 window manager detected...\n");
			return 0;
		case SDL_SYSWM_WAYLAND:
			printf("Wayland window manager detected...\n");
			return 0;
		case SDL_SYSWM_MIR:
			printf("Mir window manager detected...\n");
			return 0;
		default:
			printf("Unsupported window manager.\n");
			return 1;
		}
	}
	printf("Could not retrieve window manager information: %s", SDL_GetError());
	return 1;
}
void cleanSDL()
{
	TTF_CloseFont(sdlshit.font);
	TTF_Quit();
	SDL_DestroyWindow(sdlshit.window);
	SDL_Quit();

	printf("SDL clean up successful.\n");
}