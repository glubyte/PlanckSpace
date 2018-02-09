#pragma once

// standard
#include <stdio.h>
// local
#include "vk.h"
#include "sdl.h"
#include "fps.h"

struct {
	uint32_t ticks;
	VkDescriptorPool descPool;
	textBox options[2];
	VkCommandPool rapidPool;
	VkCommandBuffer start[2];
	VkCommandBuffer cubeCmd[2];

	VkCommandBuffer* current;
	void (*subscene)();
} menures;

// prototypes
// main
unsigned char mainMenu();
// subscene invokers
void menuStart();
// constructors
void prepareMenuResources();
void prepareTextBoxes();
void prepareStart();
void prepareCommandBuffers();
// destructors
void destroyMenuResources();
// misc
void menuWindowEvent();
void resizeMenu();
void resizeMenuPrepare();
void resizeMenuDestroy();