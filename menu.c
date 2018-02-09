#include "menu.h"

unsigned char mainMenu()
{
	// Exit Flags:
	// 0 - Exit game
	// 1 - Enter development space

	// define local resources
	unsigned char loopFlag = 1;
	unsigned char exitFlag;
	// prepare
	prepareMenuResources();
	menures.current = menures.start;
	menures.subscene = &menuStart;
	// scene loop
	while (loopFlag)
	{
		menures.ticks = SDL_GetTicks();
		// draw frame
		draw(menures.current);
		// do shit
		if ((menures.ticks - sdlshit.eventTimer) > 15)
		{
			// global updates
			sdlshit.eventTimer = menures.ticks;
			calculateFPS();
			// subscene logic
			menures.subscene();
		}
		
		//SDL_Delay(15);
		fpsshit.frames++;
	}
	// clean up
	destroyMenuResources();
	// return exit flag
	return exitFlag;
}
void menuStart()
{
	rotateZY(rotationMatrix, menures.ticks * .0025f);

	vkFreeCommandBuffers(vkshit.device, menures.rapidPool, 2, menures.cubeCmd);
	prepareCommandBuffers();

	while (SDL_PollEvent(&sdlshit.event))
	{
		switch (sdlshit.event.type)
		{
		case SDL_WINDOWEVENT:
		{
			menuWindowEvent();
			break;
		}
		case SDL_MOUSEMOTION:
		{
			if (sdlshit.event.motion.x > menures.options[0].LPixel && sdlshit.event.motion.x < menures.options[0].RPixel &&
				sdlshit.event.motion.y > menures.options[0].TPixel && sdlshit.event.motion.y < menures.options[0].BPixel)
			{
				if (menures.options[0].selected == 0)
				{
					updateUniformBuffer(menures.options[0].selectedColor, &menures.options[0].textColorUBO);
					printf("Option 1 mouseover.\n");
					menures.options[0].selected = 1;
				}
				break;
			}
			updateUniformBuffer(menures.options[0].textColor, &menures.options[0].textColorUBO);
			menures.options[0].selected = 0;

			if (sdlshit.event.motion.x > menures.options[1].LPixel && sdlshit.event.motion.x < menures.options[1].RPixel &&
				sdlshit.event.motion.y > menures.options[1].TPixel && sdlshit.event.motion.y < menures.options[1].BPixel)
			{
				if (menures.options[1].selected == 0)
				{
					updateUniformBuffer(menures.options[1].selectedColor, &menures.options[1].textColorUBO);
					printf("Option 2 mouseover.\n");
					menures.options[1].selected = 1;
				}
				break;
			}
			updateUniformBuffer(menures.options[1].textColor, &menures.options[1].textColorUBO);
			menures.options[1].selected = 0;

			break;
		}
		}
	}
}
void prepareMenuResources()
{
	VkDescriptorPoolSize poolSizes[2];
	poolSizes[0].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	poolSizes[0].descriptorCount = 2;
	poolSizes[1].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	poolSizes[1].descriptorCount = 2;
	createDescriptorPool(poolSizes, 2, 4, &menures.descPool);

	createCommandPool(VK_COMMAND_POOL_CREATE_TRANSIENT_BIT, &menures.rapidPool);

	createTextBox("Enter Development Space", 0.0f, 0.3f, (SDL_Color) { 200, 200, 200, 0 }, &menures.options[0], &menures.descPool);
	createTextBox("Exit", 0.0f, 0.5f, (SDL_Color) { 200, 200, 200, 0 }, &menures.options[1], &menures.descPool);
	// prepare secondary command buffers
	prepareCommandBuffers();

	allocateCommandBuffers(2, vkshit.commandPool, VK_COMMAND_BUFFER_LEVEL_PRIMARY, menures.start);
	for (unsigned char i = 0; i < 2; i++)
	{
		VkDeviceSize offsets[] = { 0 };
		beginCommandBuffer(VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT, menures.start[i]);
		beginRenderPass(menures.start[i], vkshit.renderPass, vkshit.framebuffers[i], (VkClearValue) { 0.1f, 0.1f, 0.1f, 1.0f });

		vkCmdBindPipeline(menures.start[i], VK_PIPELINE_BIND_POINT_GRAPHICS, vkshit.pipelines.textBox2.line);
		// draw boxes
		vkCmdBindDescriptorSets(menures.start[i], VK_PIPELINE_BIND_POINT_GRAPHICS, vkshit.pipelines.textBox2.layout, 0, 1, &menures.options[0].set, 0, NULL);
		vkCmdBindVertexBuffers(menures.start[i], 0, 1, &menures.options[0].buffer, offsets);
		vkCmdDraw(menures.start[i], 6, 1, 0, 0);

		vkCmdBindDescriptorSets(menures.start[i], VK_PIPELINE_BIND_POINT_GRAPHICS, vkshit.pipelines.textBox2.layout, 0, 1, &menures.options[1].set, 0, NULL);
		vkCmdBindVertexBuffers(menures.start[i], 0, 1, &menures.options[1].buffer, offsets);
		vkCmdDraw(menures.start[i], 6, 1, 0, 0);
		// draw cube
		vkCmdExecuteCommands(menures.start[i], 1, &menures.cubeCmd[i]);

		vkCmdEndRenderPass(menures.start[i]);
		vkEndCommandBuffer(menures.start[i]);
	}
}
void destroyMenuResources()
{
	destroyTextBox(&menures.options[1]);
	destroyTextBox(&menures.options[2]);
	vkDestroyDescriptorPool(vkshit.device, menures.descPool, NULL);
	vkFreeCommandBuffers(vkshit.device, vkshit.commandPool, 2, menures.start);
	vkFreeCommandBuffers(vkshit.device, menures.rapidPool, 2, menures.cubeCmd);
	vkDestroyCommandPool(vkshit.device, menures.rapidPool, NULL);
}
void prepareCommandBuffers()
{
	allocateCommandBuffers(2, menures.rapidPool, VK_COMMAND_BUFFER_LEVEL_SECONDARY, menures.cubeCmd);
	for (unsigned char i = 0; i < 2; i++)
	{
		VkDeviceSize offsets[] = { 0 };
		beginCommandBuffer(VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT, menures.cubeCmd[i]);

		float pc[16];
		pc[0] = projshit.view.c[0];
		pc[1] = projshit.view.c[1];
		pc[2] = projshit.view.c[2];
		pc[3] = projshit.proj.c1;
		pc[4] = projshit.proj.c2;
		pc[5] = projshit.proj.c3;
		pc[6] = projshit.proj.c4;
		pc[7] = rotationMatrix[0];
		pc[8] = rotationMatrix[1];
		pc[9] = rotationMatrix[2];
		pc[10] = rotationMatrix[3];
		pc[11] = rotationMatrix[4];
		pc[12] = rotationMatrix[5];
		pc[13] = rotationMatrix[6];
		pc[14] = rotationMatrix[7];
		pc[15] = rotationMatrix[8];

		vkCmdPushConstants(menures.cubeCmd[i], vkshit.pipelines.spinningCube.layout, VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(pc), pc);
		vkCmdBindPipeline(menures.cubeCmd[i], VK_PIPELINE_BIND_POINT_GRAPHICS, vkshit.pipelines.spinningCube.line);
		vkCmdBindVertexBuffers(menures.cubeCmd[i], 0, 1, &vkshit.primitives.cubeVert.vertexBuffer, offsets);
		vkCmdBindIndexBuffer(menures.cubeCmd[i], vkshit.primitives.cubeIndex.vertexBuffer, 0, VK_INDEX_TYPE_UINT16);
		vkCmdDrawIndexed(menures.cubeCmd[i], 36, 1, 0, 0, 0);

		vkEndCommandBuffer(menures.cubeCmd[i]);
	}
}
void menuWindowEvent()
{
	switch (sdlshit.event.window.event)
	{
		case SDL_WINDOWEVENT_RESIZED:
		{
			resizeMenu();
			break;
		}
		case SDL_WINDOWEVENT_MAXIMIZED:
		{
			SDL_SetWindowFullscreen(sdlshit.window, SDL_WINDOW_FULLSCREEN_DESKTOP);
			resizeMenu();
			break;
		}
		case SDL_WINDOWEVENT_RESTORED:
		{
			resizeMenu();
			break;
		}
	}
}
void resizeMenu()
{
	vkDeviceWaitIdle(vkshit.device);
	resizeMenuDestroy();
	resizeVkDestroy();
	resizeVkPrepare();
	resizeMenuPrepare();
}
void resizeMenuPrepare()
{
	createTextBox("Enter Development Space", 0.0f, 0.3f, (SDL_Color) { 200, 200, 200, 0 }, &menures.options[0], &menures.descPool);
	createTextBox("Exit", 0.0f, 0.5f, (SDL_Color) { 200, 200, 200, 0 }, &menures.options[1], &menures.descPool);
	prepareCommandBuffers();
	allocateCommandBuffers(2, vkshit.commandPool, VK_COMMAND_BUFFER_LEVEL_PRIMARY, menures.start);
	for (unsigned char i = 0; i < 2; i++)
	{
		VkDeviceSize offsets[] = { 0 };
		beginCommandBuffer(VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT, menures.start[i]);
		beginRenderPass(menures.start[i], vkshit.renderPass, vkshit.framebuffers[i], (VkClearValue) { 0.1f, 0.1f, 0.1f, 1.0f });

		vkCmdBindPipeline(menures.start[i], VK_PIPELINE_BIND_POINT_GRAPHICS, vkshit.pipelines.textBox.line);
		// draw boxes
		//vkCmdBindDescriptorSets(menures.start[i], VK_PIPELINE_BIND_POINT_GRAPHICS, vkshit.pipelines.textBox.layout, 0, 1, &menures.options[0].set, 0, NULL);
		vkCmdBindVertexBuffers(menures.start[i], 0, 1, &menures.options[0].buffer, offsets);
		vkCmdDraw(menures.start[i], 6, 1, 0, 0);

		//vkCmdBindDescriptorSets(menures.start[i], VK_PIPELINE_BIND_POINT_GRAPHICS, vkshit.pipelines.textBox.layout, 0, 1, &menures.options[1].set, 0, NULL);
		vkCmdBindVertexBuffers(menures.start[i], 0, 1, &menures.options[1].buffer, offsets);
		vkCmdDraw(menures.start[i], 6, 1, 0, 0);
		// draw cube
		vkCmdExecuteCommands(menures.start[i], 1, &menures.cubeCmd[i]);

		vkCmdEndRenderPass(menures.start[i]);
		vkEndCommandBuffer(menures.start[i]);
	}
}
void resizeMenuDestroy()
{
	destroyTextBox(&menures.options[0]);
	destroyTextBox(&menures.options[1]);
	vkFreeCommandBuffers(vkshit.device, menures.rapidPool, 2, menures.cubeCmd);
	vkFreeCommandBuffers(vkshit.device, vkshit.commandPool, 2, menures.start);
}