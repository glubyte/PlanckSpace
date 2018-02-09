#pragma once

// standard
#include <stdio.h>
#include <stdint.h>
#include <time.h>
// local
#include "sdl.h"

// stack handles
struct {
	uint32_t fps;
	uint32_t sdltimer;
	uint32_t frames;
	uint32_t dt;
	time_t timer;
} fpsshit;

// prototypes
void initFPS();
void calculateFPS();