#pragma once

// standard
#include <stdio.h>
// external
#include <SDL.h>
#include <SDL_syswm.h>
#include <SDL_ttf.h>

// stack handles
struct {
	SDL_SysWMinfo windowInfo;
	SDL_Event event;
	int winWidth;
	int winHeight;
	uint32_t eventTimer;
	SDL_Window* window;
	TTF_Font* font;
} sdlshit;


// prototypes
unsigned char initSDL();
unsigned char createWindow();
void cleanSDL();