unsigned char resizeWindow()
{
	unsigned char i;

	printf("Resizing window...\n");

	// wait for idle
	vkDeviceWaitIdle(vkshit.device);

	// acquire new dimensions
	SDL_GetWindowSize(sdlshit.window, &sdlshit.winWidth, &sdlshit.winHeight);

	// new projection vector
	genProj();

	// clean dependent variables
	printf("Cleaning up resources...\n");
	vkFreeCommandBuffers(vkshit.device, vkshit.commandPool, 2, vkshit.commandBuffers);
	for (i = 0; i < vkshit.imageCount; i++)
	{
		vkDestroyFramebuffer(vkshit.device, vkshit.framebuffers[i], NULL);
	}
	destroyPipelines();
	//destroyTextBox(&vkshit.textBoxes.test);
	vkDestroyRenderPass(vkshit.device, vkshit.renderPass, NULL);
	for (i = 0; i < vkshit.imageCount; i++)
	{
		vkDestroyImageView(vkshit.device, vkshit.imageViews[i], NULL);
	}
	vkDestroySwapchainKHR(vkshit.device, vkshit.swapChain, NULL); // implicitly destroys VkImage

	// generate dependent variables
	printf("Generating new resources...\n");
	if (createSwapChain())
		return 1;
	if (createImageViews())
		return 1;
	if (createRenderPass())
		return 1;
	preparePipelines();
	//createTextBox("hello jack", 0.0f, 0.0f, (SDL_Color) { 200, 200, 200, 0 }, &vkshit.textBoxes.test);

	if (createFramebuffers())
		return 1;
	if (createCommandBuffers())
		return 1;

	return 0;
}
unsigned char createPipeline(char* vertFile, char* fragFile)
{
	// load shaders
	shaderHandle vertHandle;
	shaderHandle fragHandle;
	loadShader(&vertHandle, vertFile);
	loadShader(&fragHandle, fragFile);

	VkShaderModule vertexModule;
	VkShaderModule fragModule;

	VkShaderModuleCreateInfo vertexModuleInfo;
	vertexModuleInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	vertexModuleInfo.pNext = NULL;
	vertexModuleInfo.flags = 0;
	vertexModuleInfo.codeSize = vertHandle.codeSize;
	vertexModuleInfo.pCode = vertHandle.code;
	if (assessError(vkCreateShaderModule(vkshit.device, &vertexModuleInfo, NULL, &vertexModule)))
	{
		printf("Failed to create vertex shader module.\n");
		return 1;
	}
	VkShaderModuleCreateInfo fragmentModuleInfo;
	fragmentModuleInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	fragmentModuleInfo.pNext = NULL;
	fragmentModuleInfo.flags = 0;
	fragmentModuleInfo.codeSize = fragHandle.codeSize;
	fragmentModuleInfo.pCode = fragHandle.code;
	if (assessError(vkCreateShaderModule(vkshit.device, &fragmentModuleInfo, NULL, &fragModule)))
	{
		printf("Failed to create fragment shader module.\n");
		return 1;
	}
	free(vertHandle.code);
	vertHandle.code = NULL;
	free(fragHandle.code);
	fragHandle.code = NULL;

	VkPipelineShaderStageCreateInfo vertexStageInfo;
	vertexStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	vertexStageInfo.pNext = NULL;
	vertexStageInfo.flags = 0;
	vertexStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
	vertexStageInfo.module = vertexModule;
	vertexStageInfo.pName = "main";
	vertexStageInfo.pSpecializationInfo = NULL;

	VkPipelineShaderStageCreateInfo fragStageInfo;
	fragStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	fragStageInfo.pNext = NULL;
	fragStageInfo.flags = 0;
	fragStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
	fragStageInfo.module = fragModule;
	fragStageInfo.pName = "main";
	fragStageInfo.pSpecializationInfo = NULL;

	VkPipelineShaderStageCreateInfo stages[] = { vertexStageInfo, fragStageInfo };

	// vertex input
	VkVertexInputBindingDescription vibd[2];
	vibd[0].binding = 0;
	vibd[0].stride = sizeof(float) * 6;
	vibd[0].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
	vibd[1].binding = 1;
	vibd[1].stride = sizeof(float) * 3;
	vibd[1].inputRate = VK_VERTEX_INPUT_RATE_INSTANCE;

	VkVertexInputAttributeDescription viad[3];
	viad[0].binding = 0;
	viad[0].location = 0;
	viad[0].format = VK_FORMAT_R32G32B32_SFLOAT;
	viad[0].offset = 0;
	viad[1].binding = 0;
	viad[1].location = 1;
	viad[1].format = VK_FORMAT_R32G32B32_SFLOAT;
	viad[1].offset = sizeof(float) * 3;
	viad[2].binding = 1;
	viad[2].location = 2;
	viad[2].format = VK_FORMAT_R32G32B32_SFLOAT;
	viad[2].offset = 0;

	VkPipelineVertexInputStateCreateInfo vertexInputState;
	vertexInputState.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
	vertexInputState.pNext = NULL;
	vertexInputState.flags = 0;
	vertexInputState.vertexBindingDescriptionCount = 2;
	vertexInputState.pVertexBindingDescriptions = vibd;
	vertexInputState.vertexAttributeDescriptionCount = 3;
	vertexInputState.pVertexAttributeDescriptions = viad;

	// input assembler
	VkPipelineInputAssemblyStateCreateInfo assemblyState;
	assemblyState.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
	assemblyState.pNext = NULL;
	assemblyState.flags = 0;
	assemblyState.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
	assemblyState.primitiveRestartEnable = VK_FALSE;

	// viewport
	VkViewport viewport;
	viewport.x = 0.0f;
	viewport.y = 0.0f;
	viewport.width = (float)sdlshit.winWidth;
	viewport.height = (float)sdlshit.winHeight;
	viewport.minDepth = 0.0f;
	viewport.maxDepth = 1.0f;

	VkRect2D scissor;
	VkOffset2D offset = { 0, 0 };
	VkExtent2D extent = { sdlshit.winWidth, sdlshit.winHeight };
	scissor.offset = offset;
	scissor.extent = extent;

	VkPipelineViewportStateCreateInfo viewportState;
	viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
	viewportState.pNext = NULL;
	viewportState.flags = 0;
	viewportState.viewportCount = 1;
	viewportState.pViewports = &viewport;
	viewportState.scissorCount = 1;
	viewportState.pScissors = &scissor;

	// rasterizer
	VkPipelineRasterizationStateCreateInfo rasterState;
	rasterState.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
	rasterState.pNext = NULL;
	rasterState.flags = 0;
	rasterState.depthClampEnable = VK_FALSE;
	rasterState.rasterizerDiscardEnable = VK_FALSE;
	rasterState.polygonMode = VK_POLYGON_MODE_FILL;
	rasterState.cullMode = VK_CULL_MODE_BACK_BIT;
	rasterState.frontFace = VK_FRONT_FACE_CLOCKWISE;
	rasterState.depthBiasEnable = VK_FALSE;
	rasterState.depthBiasConstantFactor = 0.0f;
	rasterState.depthBiasClamp = 0.0f;
	rasterState.depthBiasSlopeFactor = 0.0f;
	rasterState.lineWidth = 1.0f;

	// multisampling
	VkPipelineMultisampleStateCreateInfo multisampleState;
	multisampleState.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
	multisampleState.pNext = NULL;
	multisampleState.flags = 0;
	multisampleState.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
	multisampleState.sampleShadingEnable = VK_FALSE;
	multisampleState.minSampleShading = 1.0f;
	multisampleState.pSampleMask = NULL;
	multisampleState.alphaToCoverageEnable = VK_FALSE;
	multisampleState.alphaToOneEnable = VK_FALSE;

	// color blending
	VkPipelineColorBlendAttachmentState attachment;
	attachment.blendEnable = VK_TRUE;
	attachment.srcColorBlendFactor = VK_BLEND_FACTOR_ONE;
	attachment.dstColorBlendFactor = VK_BLEND_FACTOR_ONE;
	attachment.colorBlendOp = VK_BLEND_OP_ADD;
	attachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
	attachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
	attachment.alphaBlendOp = VK_BLEND_OP_ADD;
	attachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;

	VkPipelineColorBlendStateCreateInfo colorblendState;
	colorblendState.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
	colorblendState.pNext = NULL;
	colorblendState.flags = 0;
	colorblendState.logicOpEnable = VK_FALSE;
	colorblendState.logicOp = VK_LOGIC_OP_COPY;
	colorblendState.attachmentCount = 1;
	colorblendState.pAttachments = &attachment;
	colorblendState.blendConstants[0] = 0.0f;
	colorblendState.blendConstants[1] = 0.0f;
	colorblendState.blendConstants[2] = 0.0f;
	colorblendState.blendConstants[3] = 0.0f;

	// pipeline layout (uniforms and push constants)
	VkPipelineLayoutCreateInfo layoutInfo;
	VkPushConstantRange pcr;
	pcr.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
	pcr.offset = 0;
	pcr.size = sizeof(float) * 16;
	layoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	layoutInfo.pNext = NULL;
	layoutInfo.flags = 0;
	layoutInfo.setLayoutCount = 0;
	layoutInfo.pSetLayouts = NULL;
	layoutInfo.pushConstantRangeCount = 1;
	layoutInfo.pPushConstantRanges = &pcr;
	if (assessError(vkCreatePipelineLayout(vkshit.device, &layoutInfo, NULL, &vkshit.layout)))
	{
		printf("Failed to create pipeline layout.\n");
		return 1;
	}

	VkPipelineDepthStencilStateCreateInfo dss;
	dss.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
	dss.pNext = NULL;
	dss.flags = 0;
	dss.depthTestEnable = VK_TRUE;
	dss.depthWriteEnable = VK_TRUE; // transparent shit
	dss.depthCompareOp = VK_COMPARE_OP_LESS;
	dss.depthBoundsTestEnable = VK_FALSE;
	dss.stencilTestEnable = VK_FALSE;

	VkGraphicsPipelineCreateInfo pipelineInfo;
	pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
	pipelineInfo.pNext = NULL;
	pipelineInfo.flags = 0;
	pipelineInfo.stageCount = 2;
	pipelineInfo.pStages = &stages[0];
	pipelineInfo.pVertexInputState = &vertexInputState;
	pipelineInfo.pInputAssemblyState = &assemblyState;
	pipelineInfo.pTessellationState = NULL;
	pipelineInfo.pViewportState = &viewportState;
	pipelineInfo.pRasterizationState = &rasterState;
	pipelineInfo.pMultisampleState = &multisampleState;
	pipelineInfo.pDepthStencilState = &dss;
	pipelineInfo.pColorBlendState = &colorblendState;
	pipelineInfo.pDynamicState = NULL;
	pipelineInfo.layout = vkshit.layout;
	pipelineInfo.renderPass = vkshit.renderPass;
	pipelineInfo.subpass = 0;
	pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;
	pipelineInfo.basePipelineIndex = -1;

	if (assessError(vkCreateGraphicsPipelines(vkshit.device, VK_NULL_HANDLE, 1, &pipelineInfo, NULL, &vkshit.pipeline)))
	{
		printf("Unable to create pipeline.\n");
		return 1;
	}

	// clean up
	vkDestroyShaderModule(vkshit.device, vertexModule, NULL);
	vkDestroyShaderModule(vkshit.device, fragModule, NULL);

	return 0;
}
unsigned char createCommandBuffers()
{
	unsigned int i = 0;
	VkDeviceSize offsets[] = { 0 };
	VkCommandBufferAllocateInfo allocInfo;
	allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocInfo.pNext = NULL;
	allocInfo.commandPool = vkshit.commandPool;
	allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	allocInfo.commandBufferCount = 2;

	if (assessError(vkAllocateCommandBuffers(vkshit.device, &allocInfo, vkshit.commandBuffers)))
	{
		printf("Failed to allocate command buffers.\n");
		return 1;
	}

	VkCommandBufferBeginInfo commandBeginInfo;
	commandBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	commandBeginInfo.pNext = NULL;
	commandBeginInfo.flags = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;
	commandBeginInfo.pInheritanceInfo = NULL;

	VkRenderPassBeginInfo renderBeginInfo;
	VkOffset2D offset = { 0, 0 };
	VkExtent2D extent = { sdlshit.winWidth, sdlshit.winHeight };
	VkClearValue clearColor = { 0.1f, 0.1f, 0.1f, 1.0f };
	VkClearValue depthClear = { 1.0f };
	VkClearValue clear[] = { clearColor, depthClear };
	renderBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
	renderBeginInfo.pNext = NULL;
	renderBeginInfo.renderPass = vkshit.renderPass;
	renderBeginInfo.renderArea.offset = offset;
	renderBeginInfo.renderArea.extent = extent;
	renderBeginInfo.clearValueCount = 2;
	renderBeginInfo.pClearValues = clear;

	for (i = 0; i < 2; i++)
	{
		vkBeginCommandBuffer(vkshit.commandBuffers[i], &commandBeginInfo);
		renderBeginInfo.framebuffer = vkshit.framebuffers[i];
		vkCmdBeginRenderPass(vkshit.commandBuffers[i], &renderBeginInfo, VK_SUBPASS_CONTENTS_INLINE);
		/*
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
		vkCmdPushConstants(vkshit.commandBuffers[i], vkshit.pipelines.spinningCube.layout, VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(pc), pc);
		vkCmdBindPipeline(vkshit.commandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, vkshit.pipelines.spinningCube.line);
		vkCmdBindVertexBuffers(vkshit.commandBuffers[i], 0, 1, &vkshit.vertexBuffer, offsets);
		//vkCmdBindVertexBuffers(vkshit.commandBuffers[i], 1, 1, &buffers.worldMesh.vertexBuffer, offsets);
		vkCmdBindIndexBuffer(vkshit.commandBuffers[i], vkshit.indexBuffer, 0, VK_INDEX_TYPE_UINT16);
		vkCmdDrawIndexed(vkshit.commandBuffers[i], 36, 1, 0, 0, 0);
		*/
		// text box
		vkCmdBindPipeline(vkshit.commandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, vkshit.pipelines.textBox.line);
		vkCmdBindDescriptorSets(vkshit.commandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, vkshit.pipelines.textBox.layout, 0, 1, &vkshit.setLayouts.imageSamplerSet, 0, NULL);
		vkCmdBindVertexBuffers(vkshit.commandBuffers[i], 0, 1, &vkshit.textBoxes.test.buffer, offsets);
		vkCmdDraw(vkshit.commandBuffers[i], 6, 1, 0, 0);

		vkCmdEndRenderPass(vkshit.commandBuffers[i]);
		if (assessError(vkEndCommandBuffer(vkshit.commandBuffers[i])))
		{
			printf("Failed to record command buffers.\n");
			return 1;
		}
	}
	return 0;
}

while (videoFlag)
				{
					// utilize time more efficiently and precisely with time.h
					ticks = SDL_GetTicks();

					// WARNING: ENABLING THIS AND DISABLING THE RENDER LIMITER WILL MAX OUT THE GPU AND CPU. USEFUL ONLY FOR DEBUGGING AND OPTIMIZATION PURPOSES
					drawFrame();

					// There are multiple different ticks per second or TPS's: 
					// Rendering TPS, or FPS, is the rate at which the GPU renders to the screen. This is capped at 62.5 hz and can be changed in the options to no more than 120 hz to accomodate
					// high-end monitors. FPS cannot exceed 120 hz since the remaining GPU/CPU resources are used for physics and math.
					// World TPS, or TPS, is the rate at which the program processes world logic and queries for human input. This is capped at 120 hz. (arbitrary. might need more, might need less)
					// Compute TPS, or CPS, is the rate at which the program executes all required physics and math algorithms on the CPU or on the GPU via compute shaders.
					// CPS is uncapped and will commonly exceed several million hz depending on situation and hardware.

					// RENDERING SHIT
					// WARNING: ENABLING VSYNC WILL COMPLETELY FUCK THIS SHIT UP SINCE THE BULK OF GPU/CPU RESOURCES ARE USED IN PHYSICS AND COMPUTE CALCULATIONS AND LIMITING THE ENTIRE GPU
					// TO THE REFRESH RATE OF THE MONITOR IS KILL (though, this may not be the case if different swapchains work independently of one another. in fact, an entirely different logical
					// device might be used for compute shit)
					
					/*
					if ((ticks - vkshit.renderTimer) > 15)
					{
						vkshit.renderTimer = SDL_GetTicks();
						drawFrame();
					}
					*/
					
					
					// WORLD SHIT
					if ((ticks - sdlshit.eventTimer) > 10)
					{
						/*
						// update push constants and re-record associated command buffers
						vkFreeCommandBuffers(vkshit.device, vkshit.commandPool, 2, vkshit.commandBuffers);
						if (assessError(createCommandBuffers()))
						{
							printf("Failed to record command buffers.\n");
						}
						*/
						sdlshit.eventTimer = SDL_GetTicks();
						calculateFPS();
						rotateZY(rotationMatrix, ticks * .0025f);
						while (SDL_PollEvent(&sdlshit.event))
						{
							switch (sdlshit.event.type)
							{
							case SDL_KEYDOWN:
								switch (sdlshit.event.key.keysym.sym)
								{
									case SDLK_w:
									{
										projshit.view.c[1] = projshit.view.c[1] + .1f;
										break;
									}
									case SDLK_s:
									{
										projshit.view.c[1] = projshit.view.c[1] - .1f;
										break;
									}
									case SDLK_a:
									{
										projshit.view.c[0] = projshit.view.c[0] - .1f;
										break;
									}
									case SDLK_d:
									{
										projshit.view.c[0] = projshit.view.c[0] + .1f;
										break;
									}
									case SDLK_LSHIFT:
									{
										projshit.view.c[2] = projshit.view.c[2] + .1f;
										break;
									}
									case SDLK_SPACE:
									{
										projshit.view.c[2] = projshit.view.c[2] - .1f;
										break;
									}
								}
								break;
							case SDL_WINDOWEVENT:
							{
								switch (sdlshit.event.window.event)
								{
								case SDL_WINDOWEVENT_CLOSE:
									vkDeviceWaitIdle(vkshit.device);
									cleanVk();
									cleanSDL();
									videoFlag = 0;
									break;
								case SDL_WINDOWEVENT_RESIZED:
									resizeWindow();
									printf("%i %i\n", sdlshit.winWidth, sdlshit.winHeight);
									break;
								case SDL_WINDOWEVENT_MAXIMIZED:
									SDL_SetWindowFullscreen(sdlshit.window, SDL_WINDOW_FULLSCREEN_DESKTOP);
									resizeWindow();
									break;
								}
								break;
							}
							}
						}
					}
					// PHYSICS, MATH, COMPUTE SHADERS, AND REMAINING CPU/GPU RESOURCES
					// SDL_Delay(10);
					fpsshit.frames++;
				}