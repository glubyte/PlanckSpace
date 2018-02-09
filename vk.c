#include "vk.h"

unsigned char initVk()
{
	printf("Creating instance...\n");
	if (createInstance())
		return 1;
	printf("Creating surface...\n");
	if (createSurface())
		return 1;
	printf("Scanning GPU's...\n");
	if (createPhysicalDevice())
		return 1;
	printf("Creating logical device...\n");
	if (createDevice())
		return 1;
	printf("Creating swap chain...\n");
	if (createSwapChain())
		return 1;
	printf("Creating image views...\n");
	if (createImageViews())
		return 1;
	printf("Creating render pass...\n");
	if (createRenderPass())
		return 1;
	printf("Creating sampler...\n");
	if (createSampler())
		return 1;
	printf("Preparing descriptor sets...\n");
	prepareDescriptorSetLayouts();
	printf("Preparing pipelines...\n");
	preparePipelines();
	printf("Creating command pool...\n");
	createCommandPool(0, &vkshit.commandPool);
	printf("Create depth resources...\n");
	if (createDepthResources())
		return 1;
	printf("Creating framebuffers...\n");
	if (createFramebuffers())
		return 1;
	printf("Creating semaphores...\n");
	if (createSemaphores())
		return 1;

	initGeometry();
	createVertexBuffer(cube, sizeof(cube), &vkshit.primitives.cubeVert.vertexBuffer, &vkshit.primitives.cubeVert.memory);
	createIndexBuffer(cubeIndices, sizeof(cubeIndices), &vkshit.primitives.cubeIndex.vertexBuffer, &vkshit.primitives.cubeIndex.memory);
	prepareDrawInfo();
	return 0;
}
unsigned char createInstance()
{
#if defined(VK_USE_PLATFORM_WIN32_KHR)
	const char* extensionNames[] = { VK_KHR_SURFACE_EXTENSION_NAME , VK_KHR_WIN32_SURFACE_EXTENSION_NAME };
#elif defined(VK_USE_PLATFORM_XLIB_KHR)
	const char* extensionNames[] = { VK_KHR_SURFACE_EXTENSION_NAME, VK_KHR_XLIB_SURFACE_EXTENSION_NAME };
#elif defined(VK_USE_PLATFORM_WAYLAND_KHR)
	const char* extensionNames[] = { VK_KHR_SURFACE_EXTENSION_NAME, VK_KHR_WAYLAND_SURFACE_EXTENSION_NAME };
#elif defined(VK_USE_PLATFORM_MIR_KHR)
	const char* extensionNames[] = { VK_KHR_SURFACE_EXTENSION_NAME, VK_KHR_MIR_SURFACE_EXTENSION_NAME };
#endif

	VkApplicationInfo appInfo;
	appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	appInfo.pNext = NULL;
	appInfo.pApplicationName = "Planck Space";
	appInfo.applicationVersion = VK_MAKE_VERSION(0, 0, 1);
	appInfo.pEngineName = "Konstrukteur";
	appInfo.engineVersion = VK_MAKE_VERSION(0, 0, 1);
	appInfo.apiVersion = VK_MAKE_VERSION(1, 0, 65);

	VkInstanceCreateInfo instanceInfo;
	instanceInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	instanceInfo.pNext = NULL;
	instanceInfo.flags = 0;
	instanceInfo.pApplicationInfo = &appInfo;
	instanceInfo.enabledLayerCount = 0;
	instanceInfo.ppEnabledLayerNames = NULL;
	instanceInfo.enabledExtensionCount = 2;
	instanceInfo.ppEnabledExtensionNames = extensionNames;

	if(assessError(vkCreateInstance(&instanceInfo, NULL, &vkshit.instance)))
	{
		printf("Failed to create Vulkan instance.\n");
		return 1;
	}

	return 0;
}
unsigned char createSurface()
{	
#if defined(VK_USE_PLATFORM_WIN32_KHR)
	VkWin32SurfaceCreateInfoKHR surfaceInfo;
	surfaceInfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
	surfaceInfo.pNext = NULL;
	surfaceInfo.flags = 0;
	surfaceInfo.hinstance = GetModuleHandle(NULL);
	surfaceInfo.hwnd = sdlshit.windowInfo.info.win.window;

	if (assessError(vkCreateWin32SurfaceKHR(vkshit.instance, &surfaceInfo, NULL, &vkshit.surface)))
	{
		printf("Failed to create Win32 surface.\n");
		return 1;
	}
#elif defined(VK_USE_PLATFORM_XLIB_KHR)
	VkXlibSurfaceCreateInfoKHR surfaceInfo;
	surfaceInfo.sType = VK_STRUCTURE_TYPE_XLIB_SURFACE_CREATE_INFO_KHR;
	surfaceInfo.pNext = NULL;
	surfaceInfo.flags = 0;
	surfaceInfo.dpy = sdlshit.windowInfo.info.X11.display;

	if (assessError(vkCreateXlibSurfaceKHR(vkshit.instance, &surfaceInfo, NULL, &vkshit.surface)))
	{
		printf("Failed to created Xlib surface.\n");
		return 1;
	}
#elif defined(VK_USE_PLATFORM_WAYLAND_KHR)
	VkWaylandSurfaceCreateInfoKHR surfaceInfo;
	surfaceInfo.sType = VK_STRUCTURE_TYPE_WAYLAND_SURFACE_CREATE_INFO_KHR;
	surfaceInfo.pNext = NULL;
	surfaceInfo.flags = 0;
	surfaceInfo.display = sdlshit.windowInfo.info.wl.display;
	surfaceInfo.surface = sdlshit.windowInfo.info.wl.surface;

	if (assessError(vkCreateWaylandSurfaceKHR(vkshit.instance, &surfaceInfo, NULL, &vkshit.surface)))
	{
		printf("Failed to created Wayland surface.\n");
		return 1;
	}
#elif defined(VK_USE_PLATFORM_MIR_KHR)
	VkMirSurfaceCreateInfoKHR surfaceInfo;
	surfaceInfo.sType = VK_STRUCTURE_TYPE_MIR_SURFACE_CREATE_INFO_KHR;
	surfaceInfo.pNext = NULL;
	surfaceInfo.flags = 0;
	surfaceInfo.connection = sdlshit.windowInfo.info.mir.connection;
	surfaceInfo.mirSurface = sdlshit.windowInfo.info.mir.surface;

	if (assessError(vkCreateMirSurfaceKHR(vkshit.instance, &surfaceInfo, NULL, &vkshit.surface)))
	{
		printf("Failed to created Mir surface.\n");
		return 1;
	}
#endif

	return 0;
}
unsigned char createPhysicalDevice()
{
	uint32_t deviceCount = 0;
	VkPhysicalDevice* physicalDevices;
	VkPhysicalDeviceProperties deviceProperties;
	uint32_t queueFamilyPropertyCount = 0;
	VkQueueFamilyProperties* queueFamilyProperties;
	VkBool32 supported;
	unsigned char i, j;

	// enumerate physical devices
	vkEnumeratePhysicalDevices(vkshit.instance, &deviceCount, NULL);
	if (deviceCount == 0)
	{
		printf("Failed to find GPU with Vulkan support.\n");
		return 1;
	}
	physicalDevices = malloc(sizeof(VkPhysicalDevice) * deviceCount);
	vkEnumeratePhysicalDevices(vkshit.instance, &deviceCount, physicalDevices);

	// isolate compatible device and queue family
	for (i = 0; i < deviceCount; i++)
	{
		vkGetPhysicalDeviceProperties(physicalDevices[i], &deviceProperties);
		printf("Assessing %s...\n", deviceProperties.deviceName);

		if (deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
		{
			vkGetPhysicalDeviceQueueFamilyProperties(physicalDevices[i], &queueFamilyPropertyCount, NULL);
			printf("Scanning queue families...\n");

			if (queueFamilyPropertyCount > 0)
			{
				queueFamilyProperties = malloc(sizeof(VkQueueFamilyProperties) * queueFamilyPropertyCount);
				vkGetPhysicalDeviceQueueFamilyProperties(physicalDevices[i], &queueFamilyPropertyCount, queueFamilyProperties);

				for (j = 0; j < queueFamilyPropertyCount; j++)
				{
					if (queueFamilyProperties[j].queueCount > 0 && queueFamilyProperties[j].queueFlags & VK_QUEUE_GRAPHICS_BIT &&
						queueFamilyProperties[j].queueFlags & VK_QUEUE_COMPUTE_BIT)
					{
						// check for surface support
						if (assessError(vkGetPhysicalDeviceSurfaceSupportKHR(physicalDevices[i], j, vkshit.surface, &supported)))
						{
							printf("Could not retrieve surface information for family %i..\n", j);
							continue;
						}
						if (supported == VK_TRUE)
						{
							vkshit.physicalDevice = physicalDevices[i];
							vkshit.queueFamilyIndex = j;

							printf("Family %i is supported.\n", j);
							printf("Compatible device isolated: %s\n", deviceProperties.deviceName);

							free(queueFamilyProperties);
							queueFamilyProperties = NULL;
							free(physicalDevices);
							physicalDevices = NULL;
							return 0;
						}
						printf("Family %i supports compute shaders, but not the current window manager...\n", j);
						continue;
					}
					printf("Family %i does not support compute shaders...\n", j);
				}
				free(queueFamilyProperties);
				queueFamilyProperties = NULL;
			}
			printf("Ignoring %s: No supported queue families...\n", deviceProperties.deviceName);
			continue;
		}
		printf("Ignoring %s: Not a discrete GPU...\n", deviceProperties.deviceName);
	}
	// terminate program, no compatible devices found
	printf("Although there exists a Vulkan compatible GPU, it does not support the additional required features.\n");
	free(physicalDevices);
	physicalDevices = NULL;
	return 1;
}
unsigned char createDevice()
{
	const char *deviceExtensions[] = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };
	float queuePriority = 1.0f;

	VkDeviceQueueCreateInfo queueInfo;
	queueInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
	queueInfo.pNext = NULL;
	queueInfo.flags = 0;
	queueInfo.queueCount = 1;
	queueInfo.queueFamilyIndex = vkshit.queueFamilyIndex;
	queueInfo.pQueuePriorities = &queuePriority;

	VkDeviceCreateInfo deviceInfo;
	deviceInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	deviceInfo.pNext = NULL;
	deviceInfo.flags = 0;
	deviceInfo.pEnabledFeatures = NULL;
	deviceInfo.queueCreateInfoCount = 1;
	deviceInfo.pQueueCreateInfos = &queueInfo;
	deviceInfo.enabledLayerCount = 0;
	deviceInfo.ppEnabledLayerNames = NULL;
	deviceInfo.enabledExtensionCount = 1;
	deviceInfo.ppEnabledExtensionNames = deviceExtensions;
	
	if (assessError(vkCreateDevice(vkshit.physicalDevice, &deviceInfo, NULL, &vkshit.device)))
	{
		printf("Failed to create logical device.\n");
		return 1;
	}

	vkGetDeviceQueue(vkshit.device, vkshit.queueFamilyIndex, 0, &vkshit.queue);

	return 0;
}
unsigned char createSwapChain()
{
	VkSwapchainCreateInfoKHR swapchainInfo;
	VkSurfaceCapabilitiesKHR capabilities;
	unsigned char i;

	vkGetPhysicalDeviceSurfaceCapabilitiesKHR(vkshit.physicalDevice, vkshit.surface, &capabilities);

	// isolate image format and color space
	uint32_t formatCount;
	vkGetPhysicalDeviceSurfaceFormatsKHR(vkshit.physicalDevice, vkshit.surface, &formatCount, NULL);
	if (formatCount > 0)
	{
		VkSurfaceFormatKHR* formats = malloc(sizeof(VkSurfaceFormatKHR) * formatCount);
		vkGetPhysicalDeviceSurfaceFormatsKHR(vkshit.physicalDevice, vkshit.surface, &formatCount, formats);

		// check for best case scenario
		if (formatCount == 1 && formats[0].format == VK_FORMAT_UNDEFINED)
		{
			vkshit.imageFormat = VK_FORMAT_B8G8R8A8_UNORM;
			swapchainInfo.imageColorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR;
		}
		else
		{
			for (i = 0; i < formatCount; i++)
			{
				if (formats[i].format == VK_FORMAT_B8G8R8A8_UNORM && formats[i].colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
				{
					vkshit.imageFormat = VK_FORMAT_B8G8R8A8_UNORM;
					swapchainInfo.imageColorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR;
					i = 0;
					break;
				}
			}
			if (i)
			{
				printf("Could not locate ideal surface format and color space. A technological upgrade is recommended.\n");
				return 0;
			}
		}
		free(formats);
		formats = NULL;
	}
	else 
	{
		printf("No supported surface formats.\n");
		return 0;
	}

	// isolate present mode
	uint32_t presentCount;
	vkGetPhysicalDeviceSurfacePresentModesKHR(vkshit.physicalDevice, vkshit.surface, &presentCount, NULL);
	if (presentCount > 0)
	{
		swapchainInfo.presentMode = VK_PRESENT_MODE_MAILBOX_KHR;
	}
	else
	{
		printf("No supported present modes.\n");
		return 0;
	}

	// handle swap chain length
	vkshit.imageCount = 2;
	if (vkshit.imageCount > capabilities.maxImageCount)
	{
		printf("Physical device does not support necessary amount of images. A technological upgrade is recommended.\n");
		return 0;
	}

	// assign image extent
	VkExtent2D extent = { sdlshit.winWidth, sdlshit.winHeight };

	swapchainInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
	swapchainInfo.pNext = NULL;
	swapchainInfo.flags = 0;
	swapchainInfo.surface = vkshit.surface;
	swapchainInfo.minImageCount = vkshit.imageCount;
	swapchainInfo.imageFormat = vkshit.imageFormat;
	swapchainInfo.imageExtent = extent;
	swapchainInfo.imageArrayLayers = 1;
	swapchainInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
	swapchainInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
	swapchainInfo.queueFamilyIndexCount = 0;
	swapchainInfo.pQueueFamilyIndices = NULL;
	swapchainInfo.preTransform = capabilities.currentTransform;
	swapchainInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
	swapchainInfo.clipped = VK_TRUE;
	swapchainInfo.oldSwapchain = NULL;

	if (assessError(vkCreateSwapchainKHR(vkshit.device, &swapchainInfo, NULL, &vkshit.swapChain)))
	{
		printf("Failed to create swap chain.\n");
		return 1;
	}

	// get swap chain image handles
	vkGetSwapchainImagesKHR(vkshit.device, vkshit.swapChain, &vkshit.imageCount, vkshit.images);

	return 0;
}
unsigned char createImageViews()
{
	for (unsigned char i = 0; i < vkshit.imageCount; i++)
	{
		if (createImageView(vkshit.images[i], vkshit.imageFormat, VK_IMAGE_ASPECT_COLOR_BIT, &vkshit.imageViews[i]))
		{
			return 1;
		}
	}
	return 0;
}
unsigned char createRenderPass()
{
	VkAttachmentDescription colorAttachment;
	colorAttachment.flags = 0;
	colorAttachment.format = vkshit.imageFormat;
	colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
	colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
	colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

	VkAttachmentDescription depthAttachment;
	depthAttachment.flags = 0;
	depthAttachment.format = VK_FORMAT_D32_SFLOAT;
	depthAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
	depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	depthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	depthAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	depthAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	depthAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	depthAttachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

	VkAttachmentReference colorRef;
	colorRef.attachment = 0;
	colorRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

	VkAttachmentReference depthRef;
	depthRef.attachment = 1;
	depthRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

	VkSubpassDescription subpass;
	subpass.flags = 0;
	subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
	subpass.inputAttachmentCount = 0;
	subpass.pInputAttachments = NULL;
	subpass.colorAttachmentCount = 1;
	subpass.pColorAttachments = &colorRef;
	subpass.pResolveAttachments = NULL;
	subpass.pDepthStencilAttachment = &depthRef;
	subpass.preserveAttachmentCount = 0;
	subpass.pPreserveAttachments = NULL;

	VkSubpassDependency dependency;
	dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
	dependency.dstSubpass = 0;
	dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	dependency.srcAccessMask = 0;
	dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
	dependency.dependencyFlags = 0;

	VkRenderPassCreateInfo renderPassInfo;
	VkAttachmentDescription attachments[] = { colorAttachment, depthAttachment };
	renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
	renderPassInfo.pNext = NULL;
	renderPassInfo.flags = 0;
	renderPassInfo.attachmentCount = 2;
	renderPassInfo.pAttachments = attachments;
	renderPassInfo.subpassCount = 1;
	renderPassInfo.pSubpasses = &subpass;
	renderPassInfo.dependencyCount = 1;
	renderPassInfo.pDependencies = &dependency;

	if (assessError(vkCreateRenderPass(vkshit.device, &renderPassInfo, NULL, &vkshit.renderPass)))
	{
		printf("Failed to create render pass.\n");
		return 1;
	}

	return 0;
}
unsigned char createFramebuffers()
{
	unsigned char i = 0;

	VkFramebufferCreateInfo framebufferInfo;
	framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
	framebufferInfo.pNext = NULL;
	framebufferInfo.flags = 0;
	framebufferInfo.renderPass = vkshit.renderPass;
	framebufferInfo.attachmentCount = 2;
	framebufferInfo.width = sdlshit.winWidth;
	framebufferInfo.height = sdlshit.winHeight;
	framebufferInfo.layers = 1;

	for (i = 0; i < vkshit.imageCount; i++)
	{
		VkImageView attachments[] = { vkshit.imageViews[i] , vkshit.depthImageView};

		framebufferInfo.pAttachments = attachments;

		if (assessError(vkCreateFramebuffer(vkshit.device, &framebufferInfo, NULL, &vkshit.framebuffers[i])))
		{
			printf("Failed to create framebuffers.\n");
			return 1;
		}
	}

	return 0;
}
unsigned char createSemaphores()
{
	VkSemaphoreCreateInfo semaphoreInfo;
	semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
	semaphoreInfo.pNext = NULL;
	semaphoreInfo.flags = 0;

	if (assessError(vkCreateSemaphore(vkshit.device, &semaphoreInfo, NULL, &vkshit.iSemaphore)) || assessError(vkCreateSemaphore(vkshit.device, &semaphoreInfo, NULL, &vkshit.fSemaphore)))
	{
		printf("Failed to create semaphores.\n");
		return 1;
	}
	return 0;
}
unsigned char createSampler()
{
	VkSamplerCreateInfo si;
	si.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
	si.pNext = NULL;
	si.flags = 0;
	si.magFilter = VK_FILTER_NEAREST;
	si.minFilter = VK_FILTER_NEAREST;
	si.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
	si.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
	si.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT; 
	si.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
	si.mipLodBias = 0.0f;
	si.minLod = 0.0f;
	si.maxLod = 1.0f;
	si.anisotropyEnable = VK_FALSE;
	//si.compareEnable = VK_FALSE;
	si.compareOp = VK_COMPARE_OP_NEVER;
	si.borderColor = VK_BORDER_COLOR_FLOAT_OPAQUE_WHITE;
	si.unnormalizedCoordinates = VK_FALSE;

	if (assessError(vkCreateSampler(vkshit.device, &si, NULL, &vkshit.sampler)))
	{
		printf("Failed to create sampler.\n");
		return 1;
	}

	return 0;
}
void initGeometry()
{
	float cubeTemp[] = {
		-0.5f, -0.5f, -0.5f, 0.0f, 0.0f, 0.0f,
		-0.5f,  0.5f, -0.5f, 0.0f, 0.0f, 1.0f,
		0.5f,  0.5f, -0.5f, 0.0f, 1.0f, 1.0f,
		0.5f, -0.5f, -0.5f, 1.0f, 1.0f, 1.0f,
		-0.5f, -0.5f,  0.5f, 0.0f, 0.0f, 0.0f,
		-0.5f,  0.5f,  0.5f, 0.0f, 0.0f, 1.0f,
		0.5f,  0.5f,  0.5f, 0.0f, 1.0f, 1.0f,
		0.5f, -0.5f,  0.5f, 1.0f, 1.0f, 1.0f
	};
	uint16_t cubeIndicesTemp[] = {
		0, 1, 2,
		2, 3, 0,
		3, 2, 6,
		6, 7, 3,
		7, 6, 5,
		5, 4, 7,
		4, 5, 1,
		1, 0, 4,
		1, 5, 6,
		6, 2, 1,
		4, 0, 3,
		3, 7, 4
	};
	memcpy(cube, cubeTemp, sizeof(cube));
	memcpy(cubeIndices, cubeIndicesTemp, sizeof(cubeIndices));

	float pyramidTemp[] = {
		 0.0f,  0.5f, -0.5f, 0.0f, 0.0f, 1.0f,
		 0.5f, -0.5f, -0.5f, 0.0f, 1.0f, 1.0f,
		-0.5f, -0.5f, -0.5f, 1.0f, 1.0f, 1.0f,
		 0.0f,  0.0f,  5.0f, 0.0f, 0.0f, 0.0f
	};
	uint16_t pyramidIndicesTemp[] = {
		0, 3, 1,
		1, 3, 2,
		2, 3, 0,
		0, 1, 2
	};
	memcpy(pyramid, pyramidTemp, sizeof(pyramid));
	memcpy(pyramidIndices, pyramidIndicesTemp, sizeof(pyramidIndices));

	float triangleTemp[] = {
		 0.0f,  0.5f, 0.0f, 0.0f, 0.0f, 1.0f,
		 0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 1.0f,
		-0.5f, -0.5f, 0.0f, 1.0f, 1.0f, 1.0f
	};
	memcpy(triangle, triangleTemp, sizeof(triangle));

	float squareTemp[] = {
		0.5f, 0.5f, 0.0f, 0.0f, 0.0f, 0.0f,
		-0.5f, 0.5f, 0.0f, 0.0f, 0.0f, 1.0f,
		-0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 1.0f,
		0.5f, -0.5f, 0.0f, 1.0f, 1.0f, 1.0f
	};
	uint16_t squareIndicesTemp[] = {
		0, 1, 2,
		2, 3, 0
	};
	memcpy(square, squareTemp, sizeof(square));
	memcpy(squareIndices, squareIndicesTemp, sizeof(squareIndices));

	float pentagonTemp[] = {
		0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f,
		0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f,
		-.951056f, .309016f, 0.0f, 0.0f, 0.0f, 1.0f,
		-.587785f, -.809017f, 0.0f, 0.0f, 1.0f, 1.0f,
		.587787f, -.809017f, 0.0f, 1.0f, 1.0f, 1.0f,
		.951056f, .309016f, 0.0f, 0.0f, 1.0f, 1.0f
	};
	uint16_t pentagonIndicesTemp[] = {
		0, 1, 2,
		0, 2, 3,
		0, 3, 4,
		0, 4, 5,
		0, 5, 1
	};
	memcpy(pentagon, pentagonTemp, sizeof(pentagon));
	memcpy(pentagonIndices, pentagonIndicesTemp, sizeof(pentagonIndices));

	float worldMeshTemp[] = {
		-0.5f, 0.0f, 0.0f,
		 0.5f, 0.0f, 0.0f,
		 1.5f, 0.0f, 0.0f,
		 -0.5f, 0.0f, 1.0f,
		 0.5f, 0.0f, 1.0f,
		 1.5f, 0.0f, 1.0f,
		 -0.5f, 0.0f, 2.0f,
		 0.5f, 0.0f, 2.0f,
		 1.5f, 0.0f, 2.0f
	};
	memcpy(worldMesh.pos, worldMeshTemp, sizeof(worldMesh));
	
}

void createVFPipeline(char* vertexShader, char* fragmentShader, VkVertexInputBindingDescription* vibd, uint32_t vibdNum, VkVertexInputAttributeDescription* viad, uint32_t viadNum,
	VkPipelineLayout layout, VkPipeline* pipeline)
{
	// For this abstract VF (Vertex-Fragment) pipeline generator, settings are optimized for simplicity and efficiency and should be valid for nearly all VF vkshit.pipelines.
	// Input assembler is a triangle list, viewport and scissor are equal to window dimensions, depth clamping is disabled, polygons are filled, multisampling is disabled, 
	// depth testing is enabled, and color blending is disabled. 

	shaderHandle vertHandle;
	shaderHandle fragHandle;
	loadShader(&vertHandle, vertexShader);
	loadShader(&fragHandle, fragmentShader);

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
		return;
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
		return;
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

	VkPipelineVertexInputStateCreateInfo vertexInputState;
	vertexInputState.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
	vertexInputState.pNext = NULL;
	vertexInputState.flags = 0;
	vertexInputState.vertexBindingDescriptionCount = vibdNum;
	vertexInputState.pVertexBindingDescriptions = vibd;
	vertexInputState.vertexAttributeDescriptionCount = viadNum;
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
	attachment.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
	attachment.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
	attachment.colorBlendOp = VK_BLEND_OP_ADD;
	attachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
	attachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
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
	pipelineInfo.layout = layout;
	pipelineInfo.renderPass = vkshit.renderPass;
	pipelineInfo.subpass = 0;
	pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;
	pipelineInfo.basePipelineIndex = -1;

	if (assessError(vkCreateGraphicsPipelines(vkshit.device, VK_NULL_HANDLE, 1, &pipelineInfo, NULL, pipeline)))
	{
		printf("Unable to create pipeline.\n");
		return;
	}

	// clean up
	vkDestroyShaderModule(vkshit.device, vertexModule, NULL);
	vkDestroyShaderModule(vkshit.device, fragModule, NULL);
}
void createDescriptorPool(VkDescriptorPoolSize* poolSizes, uint32_t poolSizeCount, uint32_t maxSets, VkDescriptorPool* pool)
{
	VkDescriptorPoolCreateInfo dpi;
	dpi.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	dpi.pNext = NULL;
	dpi.flags = 0;
	dpi.maxSets = maxSets;
	dpi.poolSizeCount = poolSizeCount;
	dpi.pPoolSizes = poolSizes;

	vkCreateDescriptorPool(vkshit.device, &dpi, NULL, pool);
}
void createDescriptorSetLayout(uint32_t binding, VkDescriptorType type, uint32_t descriptorCount, VkShaderStageFlags stageFlags, VkDescriptorSetLayout* setLayout)
{
	VkDescriptorSetLayoutBinding dslb;
	dslb.binding = binding;
	dslb.descriptorType = type;
	dslb.descriptorCount = descriptorCount;
	dslb.stageFlags = stageFlags;
	dslb.pImmutableSamplers = NULL;

	VkDescriptorSetLayoutCreateInfo dsli;
	dsli.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	dsli.pNext = NULL;
	dsli.flags = 0;
	dsli.bindingCount = 1;
	dsli.pBindings = &dslb;

	vkCreateDescriptorSetLayout(vkshit.device, &dsli, NULL, setLayout);
}
void createDescriptorSetLayout2(VkDescriptorSetLayoutBinding* bindings, uint32_t bindingCount, VkDescriptorSetLayout* layout)
{
	VkDescriptorSetLayoutCreateInfo dsli;
	dsli.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	dsli.pNext = NULL;
	dsli.flags = 0;
	dsli.bindingCount = bindingCount;
	dsli.pBindings = bindings;

	vkCreateDescriptorSetLayout(vkshit.device, &dsli, NULL, layout);
}
void allocateDescriptorSet(VkDescriptorPool pool, VkDescriptorSetLayout* setLayout, VkDescriptorSet* set)
{
	VkDescriptorSetAllocateInfo allocInfo;
	allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	allocInfo.pNext = NULL;
	allocInfo.descriptorPool = pool;
	allocInfo.descriptorSetCount = 1;
	allocInfo.pSetLayouts = setLayout;

	vkAllocateDescriptorSets(vkshit.device, &allocInfo, set);
}
void updateDescriptorSetImage(uint32_t binding, VkSampler sampler, VkImageView imageView, VkDescriptorSet set)
{
	VkDescriptorImageInfo image;
	image.sampler = sampler;
	image.imageView = imageView;
	image.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

	VkWriteDescriptorSet write;
	write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	write.pNext = NULL;
	write.dstSet = set;
	write.dstBinding = binding;
	write.dstArrayElement = 0;
	write.descriptorCount = 1;
	write.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	write.pImageInfo = &image;
	write.pBufferInfo = NULL;
	write.pTexelBufferView = NULL;

	vkUpdateDescriptorSets(vkshit.device, 1, &write, 0, NULL);
}
void updateDescriptorSetUBO(uint32_t binding, UBO* UBO, VkDescriptorSet set)
{
	VkDescriptorBufferInfo bufferInfo;
	bufferInfo.buffer = UBO->buffer;
	bufferInfo.offset = 0;
	bufferInfo.range = UBO->size;

	VkWriteDescriptorSet write;
	write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	write.pNext = NULL;
	write.dstSet = set;
	write.dstBinding = binding;
	write.dstArrayElement = 0;
	write.descriptorCount = 1;
	write.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	write.pImageInfo = NULL;
	write.pBufferInfo = &bufferInfo;
	write.pTexelBufferView = NULL;

	vkUpdateDescriptorSets(vkshit.device, 1, &write, 0, NULL);
}
void createCommandPool(VkCommandPoolCreateFlags flags, VkCommandPool* pool)
{
	VkCommandPoolCreateInfo commandPoolInfo;
	commandPoolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	commandPoolInfo.pNext = NULL;
	commandPoolInfo.flags = flags;
	commandPoolInfo.queueFamilyIndex = vkshit.queueFamilyIndex;

	vkCreateCommandPool(vkshit.device, &commandPoolInfo, NULL, pool);
}
void allocateCommandBuffers(uint32_t count, VkCommandPool pool, VkCommandBufferLevel level, VkCommandBuffer* cmds)
{
	VkCommandBufferAllocateInfo allocInfo;
	allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocInfo.pNext = NULL;
	allocInfo.commandPool = pool;
	allocInfo.level = level;
	allocInfo.commandBufferCount = count;

	vkAllocateCommandBuffers(vkshit.device, &allocInfo, cmds);
}
void beginCommandBuffer(VkCommandBufferUsageFlags usage, VkCommandBuffer cmd)
{
	VkCommandBufferBeginInfo beginInfo;
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	beginInfo.pNext = NULL;
	beginInfo.flags = usage;
	beginInfo.pInheritanceInfo = NULL;

	vkBeginCommandBuffer(cmd, &beginInfo);
}
void createUniformBuffer(VkDeviceSize size, UBO* UBO)
{
	UBO->size = size;
	createBuffer(size, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, &UBO->buffer, &UBO->mem);
}
void updateUniformBuffer(void* data, UBO* UBO)
{
	void* map;
	vkMapMemory(vkshit.device, UBO->mem, 0, UBO->size, 0, &map);
	memcpy(map, data, UBO->size);
	vkUnmapMemory(vkshit.device, UBO->mem);
}
void draw(VkCommandBuffer* cmds)
{
	uint32_t imageIndex;
	vkAcquireNextImageKHR(vkshit.device, vkshit.swapChain, ULLONG_MAX, vkshit.iSemaphore, VK_NULL_HANDLE, &imageIndex);

	VkPipelineStageFlags waitStages = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	vkshit.submitInfo.pWaitDstStageMask = &waitStages;
	vkshit.submitInfo.pCommandBuffers = &cmds[imageIndex];
	vkQueueSubmit(vkshit.queue, 1, &vkshit.submitInfo, VK_NULL_HANDLE);

	vkshit.presentInfo.pImageIndices = &imageIndex;
	vkQueuePresentKHR(vkshit.queue, &vkshit.presentInfo);
}
void resizeVkPrepare()
{
	SDL_GetWindowSize(sdlshit.window, &sdlshit.winWidth, &sdlshit.winHeight);
	genProj();
	createSwapChain();
	createImageViews();
	createRenderPass();
	createFramebuffers();
	createDepthResources();
	preparePipelines();
}
void resizeVkDestroy()
{
	destroyPipelines();
	destroyDepthResources();
	vkDestroyFramebuffer(vkshit.device, vkshit.framebuffers[0], NULL);
	vkDestroyFramebuffer(vkshit.device, vkshit.framebuffers[1], NULL);
	vkDestroyRenderPass(vkshit.device, vkshit.renderPass, NULL);
	vkDestroyImageView(vkshit.device, vkshit.imageViews[0], NULL);
	vkDestroyImageView(vkshit.device, vkshit.imageViews[1], NULL);
	vkDestroySwapchainKHR(vkshit.device, vkshit.swapChain, NULL);
}
void beginRenderPass(VkCommandBuffer cmd, VkRenderPass renderPass, VkFramebuffer framebuffer, VkClearValue clearColor)
{
	VkRenderPassBeginInfo beginInfo;
	beginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
	beginInfo.pNext = NULL;
	beginInfo.renderPass = renderPass;
	beginInfo.framebuffer = framebuffer;
	VkRect2D ra;
	ra.extent = (VkExtent2D){ sdlshit.winWidth, sdlshit.winHeight };
	ra.offset = (VkOffset2D) { 0, 0 };
	beginInfo.renderArea = ra;
	VkClearValue depthClear = { 1.0f };
	VkClearValue clear[] = { clearColor, depthClear };
	beginInfo.clearValueCount = 2;
	beginInfo.pClearValues = clear;

	vkCmdBeginRenderPass(cmd, &beginInfo, VK_SUBPASS_CONTENTS_INLINE);
}
unsigned char createVertexBuffer(float* vertices, VkDeviceSize size, VkBuffer* buffer, VkDeviceMemory* bufferMemory)
{
	// create mappable vertex buffer on cpu and send it to gpu via staging buffer
	// create cpu buffer and copy local data
	VkBuffer stagingBuffer;
	VkDeviceMemory stagingBufferMem;
	if (createBuffer(size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, &stagingBuffer, &stagingBufferMem))
	{
		return 1;
	}
	void* data;
	vkMapMemory(vkshit.device, stagingBufferMem, 0, size, 0, &data);
	memcpy(data, vertices, size);
	vkUnmapMemory(vkshit.device, stagingBufferMem);

	// create gpu buffer
	if (createBuffer(size, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, buffer, bufferMemory))
	{
		return 1;
	}

	// copy buffer
	copyBuffer(stagingBuffer, *buffer, size);

	vkDestroyBuffer(vkshit.device, stagingBuffer, NULL);
	vkFreeMemory(vkshit.device, stagingBufferMem, NULL);

	return 0;
}
unsigned char createMeshBuffer(vec3* mesh, VkDeviceSize size, VkBuffer* buffer, VkDeviceMemory* bufferMemory)
{
	VkBuffer stagingBuffer;
	VkDeviceMemory stagingBufferMem;
	if (createBuffer(size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, &stagingBuffer, &stagingBufferMem))
	{
		return 1;
	}
	void* data;
	vkMapMemory(vkshit.device, stagingBufferMem, 0, size, 0, &data);
	memcpy(data, mesh, size);
	vkUnmapMemory(vkshit.device, stagingBufferMem);

	// create gpu buffer
	if (createBuffer(size, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, buffer, bufferMemory))
	{
		return 1;
	}

	// copy buffer
	copyBuffer(stagingBuffer, *buffer, size);

	vkDestroyBuffer(vkshit.device, stagingBuffer, NULL);
	vkFreeMemory(vkshit.device, stagingBufferMem, NULL);

	return 0;
}
unsigned char createIndexBuffer(uint16_t* indices, VkDeviceSize size, VkBuffer* buffer, VkDeviceMemory* bufferMemory)
{
	VkBuffer stagingBuffer;
	VkDeviceMemory stagingBufferMem;
	if (createBuffer(size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, &stagingBuffer, &stagingBufferMem))
	{
		return 1;
	}
	void* data;
	vkMapMemory(vkshit.device, stagingBufferMem, 0, size, 0, &data);
	memcpy(data, indices, size);
	vkUnmapMemory(vkshit.device, stagingBufferMem);

	if (createBuffer(size, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, buffer, bufferMemory))
	{
		return 1;
	}

	copyBuffer(stagingBuffer, *buffer, size);

	vkDestroyBuffer(vkshit.device, stagingBuffer, NULL);
	vkFreeMemory(vkshit.device, stagingBufferMem, NULL);

	return 0;
}
unsigned char createDepthResources()
{
	createImage(sdlshit.winWidth, sdlshit.winHeight, VK_FORMAT_D32_SFLOAT, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
		&vkshit.depthImage, &vkshit.depthImageMemory);
	createImageView(vkshit.depthImage, VK_FORMAT_D32_SFLOAT, VK_IMAGE_ASPECT_DEPTH_BIT, &vkshit.depthImageView);
	transitionImageLayout(vkshit.depthImage, VK_FORMAT_D32_SFLOAT, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL);
	return 0;
}
void destroyDepthResources()
{
	vkDestroyImageView(vkshit.device, vkshit.depthImageView, NULL);
	vkDestroyImage(vkshit.device, vkshit.depthImage, NULL);
	vkFreeMemory(vkshit.device, vkshit.depthImageMemory, NULL);
}
void prepareDrawInfo()
{
	vkshit.submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	vkshit.submitInfo.pNext = NULL;
	vkshit.submitInfo.waitSemaphoreCount = 1;
	vkshit.submitInfo.pWaitSemaphores = &vkshit.iSemaphore;
	vkshit.submitInfo.commandBufferCount = 1;
	vkshit.submitInfo.signalSemaphoreCount = 1;
	vkshit.submitInfo.pSignalSemaphores = &vkshit.fSemaphore;

	vkshit.presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
	vkshit.presentInfo.pNext = NULL;
	vkshit.presentInfo.waitSemaphoreCount = 1;
	vkshit.presentInfo.pWaitSemaphores = &vkshit.fSemaphore;
	vkshit.presentInfo.swapchainCount = 1;
	vkshit.presentInfo.pSwapchains = &vkshit.swapChain;
	vkshit.presentInfo.pResults = NULL;
}
void loadShader(shaderHandle* handle, char* file)
{
	FILE* shader = fopen(file, "rb");

	fseek(shader, 0, SEEK_END);
	handle->codeSize = (size_t)ftell(shader);
	rewind(shader);

	handle->code = (uint32_t*)malloc(handle->codeSize);
	fread(handle->code, 1, handle->codeSize, shader);

	fclose(shader);
}
unsigned char createImageView(VkImage image, VkFormat format, VkImageAspectFlags aspectMask, VkImageView* imageView)
{
	VkImageViewCreateInfo iv;
	iv.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
	iv.pNext = NULL;
	iv.flags = 0;
	iv.image = image;
	iv.viewType = VK_IMAGE_VIEW_TYPE_2D;
	iv.format = format;
	iv.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
	iv.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
	iv.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
	iv.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
	iv.subresourceRange.aspectMask = aspectMask;
	iv.subresourceRange.baseMipLevel = 0;
	iv.subresourceRange.levelCount = 1;
	iv.subresourceRange.baseArrayLayer = 0;
	iv.subresourceRange.layerCount = 1;

	if (assessError(vkCreateImageView(vkshit.device, &iv, NULL, imageView)))
	{
		printf("Failed to create image view.\n");
		return 1;
	}
	return 0;
}
unsigned char createImage(uint32_t width, uint32_t height, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties, VkImage* image, VkDeviceMemory* imageMemory)
{
	VkImageCreateInfo imageInfo;
	imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
	imageInfo.pNext = NULL;
	imageInfo.flags = 0;
	imageInfo.imageType = VK_IMAGE_TYPE_2D;
	imageInfo.format = format;
	imageInfo.extent.width = width;
	imageInfo.extent.height = height;
	imageInfo.extent.depth = 1;
	imageInfo.mipLevels = 1;
	imageInfo.arrayLayers = 1;
	imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
	imageInfo.tiling = tiling;
	imageInfo.usage = usage;
	imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	imageInfo.queueFamilyIndexCount = 0;
	imageInfo.pQueueFamilyIndices = NULL;
	imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	
	if (assessError(vkCreateImage(vkshit.device, &imageInfo, NULL, image)))
	{
		printf("Failed to create image.\n");
		return 1;
	}

	VkMemoryRequirements memReq;
	vkGetImageMemoryRequirements(vkshit.device, *image, &memReq);

	VkPhysicalDeviceMemoryProperties memProp;
	vkGetPhysicalDeviceMemoryProperties(vkshit.physicalDevice, &memProp);
	uint32_t i = 0;
	for (i = 0; i < memProp.memoryTypeCount; i++)
	{
		if (memReq.memoryTypeBits & (1 << i) && ((memProp.memoryTypes[i].propertyFlags & properties) == properties))
		{
			break;
		}
	}

	VkMemoryAllocateInfo allocInfo;
	allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	allocInfo.pNext = NULL;
	allocInfo.allocationSize = memReq.size;
	allocInfo.memoryTypeIndex = i;

	if (assessError(vkAllocateMemory(vkshit.device, &allocInfo, NULL, imageMemory)))
	{
		printf("Failed to allocate image memory.\n");
		return 1;
	}

	vkBindImageMemory(vkshit.device, *image, *imageMemory, 0);

	return 0;
}
unsigned char createBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer* buffer, VkDeviceMemory* bufferMemory)
{
	VkBufferCreateInfo bufferInfo;
	bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	bufferInfo.pNext = NULL;
	bufferInfo.flags = 0;
	bufferInfo.size = size;
	bufferInfo.usage = usage;
	bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	if (assessError(vkCreateBuffer(vkshit.device, &bufferInfo, NULL, buffer)))
	{
		printf("Failed to create buffer.\n");
		return 1;
	}
	
	VkMemoryRequirements memReq;
	vkGetBufferMemoryRequirements(vkshit.device, *buffer, &memReq);
	VkPhysicalDeviceMemoryProperties memProp;
	vkGetPhysicalDeviceMemoryProperties(vkshit.physicalDevice, &memProp);
	uint32_t i = 0;
	for (i = 0; i < memProp.memoryTypeCount; i++)
	{
		if (memReq.memoryTypeBits & (1 << i) && ((memProp.memoryTypes[i].propertyFlags & properties) == properties))
		{
			break;
		}
	}
	VkMemoryAllocateInfo allocInfo;
	allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	allocInfo.pNext = NULL;
	allocInfo.allocationSize = memReq.size;
	allocInfo.memoryTypeIndex = i;
	if (assessError(vkAllocateMemory(vkshit.device, &allocInfo, NULL, bufferMemory)))
	{
		printf("Failed to allocate buffer memory.\n");
		return 1;
	}

	vkBindBufferMemory(vkshit.device, *buffer, *bufferMemory, 0);
	
	return 0;
}
unsigned char copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size)
{
	// create a separate command pool for transient command bufffers with VK_COMMAND_POOL_CREATE_TRANSIENT_BIT flag
	VkCommandBuffer commandBuffer;
	beginSingleTimeCommands(&commandBuffer);

	VkBufferCopy copyRegion;
	copyRegion.srcOffset = 0;
	copyRegion.dstOffset = 0;
	copyRegion.size = size;

	vkCmdCopyBuffer(commandBuffer, srcBuffer, dstBuffer, 1, &copyRegion);

	endSingleTimeCommands(&commandBuffer);
	return 0;
}
void createTextBox(char* text, float xpos, float ypos, SDL_Color color, textBox* box, VkDescriptorPool* pool)
{
	// xpos within {-1, 1}
	// ypos within {-1, 1}
	box->pool = pool;
	box->selected = 0;

	SDL_Surface* surface = TTF_RenderText_Blended(sdlshit.font, text, color);
	float textColor[] = { .6f, .6f, .6f };
	float selectedColor[] = { 1.0f, 1.0f, 1.0f };
	memcpy(box->textColor, textColor, sizeof(textColor));
	memcpy(box->selectedColor, selectedColor, sizeof(selectedColor));

	VkBuffer sb;
	VkDeviceMemory sbMem;
	VkDeviceSize imageSize = surface->w * surface->h * 4;
	createBuffer(imageSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, &sb, &sbMem);
	void* data;
	vkMapMemory(vkshit.device, sbMem, 0, imageSize, 0, &data);
	memcpy(data, surface->pixels, imageSize);
	vkUnmapMemory(vkshit.device, sbMem);

	createImage(surface->w, surface->h, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, 
		&box->image.data, &box->image.mem);
	transitionImageLayout(box->image.data, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
	copyBufferToImage(sb, box->image.data, surface->w, surface->h);
	transitionImageLayout(box->image.data, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

	// pixel data for sdl selection
	box->RPixel = (sdlshit.winWidth / 2) * (1 + xpos) + surface->w / 2;
	box->LPixel = (sdlshit.winWidth / 2) * (1 + xpos) - surface->w / 2;
	box->BPixel = (sdlshit.winHeight / 2) * (1 + ypos) + surface->h / 2;
	box->TPixel = (sdlshit.winHeight / 2) * (1 + ypos) - surface->h / 2;

	float dx = ((float)surface->w / 2) / (sdlshit.winWidth / 2);
	float dy = ((float)surface->h / 2) / (sdlshit.winHeight / 2);
	float p[] = { 
		 xpos - dx, ypos - dy, 0.0f, 0.0f, 
		 xpos + dx, ypos - dy, 1.0f, 0.0f, 
		 xpos + dx, ypos + dy, 1.0f, 1.0f,

		 xpos + dx, ypos + dy, 1.0f, 1.0f,
		 xpos - dx, ypos + dy, 0.0f, 1.0f,
		 xpos - dx, ypos - dy, 0.0f, 0.0f
	};
	memcpy(box->quad, p, sizeof(p));
	
	vkDestroyBuffer(vkshit.device, sb, NULL);
	vkFreeMemory(vkshit.device, sbMem, NULL);

	SDL_FreeSurface(surface);

	createImageView(box->image.data, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_ASPECT_COLOR_BIT, &box->image.view);

	createVertexBuffer(box->quad, sizeof(box->quad), &box->buffer, &box->memory);

	createUniformBuffer(sizeof(textColor), &box->textColorUBO);
	updateUniformBuffer(textColor, &box->textColorUBO);

	allocateDescriptorSet(*pool, &vkshit.setLayouts.textBox2layout, &box->set);
	updateDescriptorSetImage(0, vkshit.sampler, box->image.view, box->set);
	updateDescriptorSetUBO(1, &box->textColorUBO, box->set);
}
void copyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height)
{
	VkCommandBuffer cmd;
	beginSingleTimeCommands(&cmd);

	VkBufferImageCopy region;
	region.bufferOffset = 0;
	region.bufferRowLength = 0;
	region.bufferImageHeight = 0;
	region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	region.imageSubresource.mipLevel = 0;
	region.imageSubresource.baseArrayLayer = 0;
	region.imageSubresource.layerCount = 1;
	region.imageOffset = (VkOffset3D){ 0, 0, 0 };
	region.imageExtent = (VkExtent3D){ width, height, 1 };

	vkCmdCopyBufferToImage(cmd, buffer, image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);

	endSingleTimeCommands(&cmd);
}
void beginSingleTimeCommands(VkCommandBuffer* commandBuffer)
{
	VkCommandBufferAllocateInfo allocInfo;
	allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocInfo.pNext = NULL;
	allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	allocInfo.commandPool = vkshit.commandPool;
	allocInfo.commandBufferCount = 1;

	vkAllocateCommandBuffers(vkshit.device, &allocInfo, commandBuffer);

	VkCommandBufferBeginInfo beginInfo;
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	beginInfo.pNext = NULL;
	beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
	beginInfo.pInheritanceInfo = NULL;

	vkBeginCommandBuffer(*commandBuffer, &beginInfo);
}
void endSingleTimeCommands(VkCommandBuffer* commandBuffer)
{
	vkEndCommandBuffer(*commandBuffer);

	VkSubmitInfo submitInfo;
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submitInfo.pNext = NULL;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = commandBuffer;
	submitInfo.waitSemaphoreCount = 0;
	submitInfo.signalSemaphoreCount = 0;

	vkQueueSubmit(vkshit.queue, 1, &submitInfo, VK_NULL_HANDLE);
	vkQueueWaitIdle(vkshit.queue);

	vkFreeCommandBuffers(vkshit.device, vkshit.commandPool, 1, commandBuffer);
}
void transitionImageLayout(VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout)
{
	VkCommandBuffer commandBuffer;
	beginSingleTimeCommands(&commandBuffer);

	VkImageMemoryBarrier imb;
	VkPipelineStageFlags srcStage;
	VkPipelineStageFlags dstStage;
	imb.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
	imb.pNext = NULL;
	imb.oldLayout = oldLayout;
	imb.newLayout = newLayout;
	imb.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	imb.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	imb.image = image;
	imb.subresourceRange.baseMipLevel = 0;
	imb.subresourceRange.levelCount = 1;
	imb.subresourceRange.baseArrayLayer = 0;
	imb.subresourceRange.layerCount = 1;

	if (newLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL) 
	{
		imb.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
	} 
	else 
	{
		imb.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	}

	if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED)
	{
		if (newLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL)
		{
			imb.srcAccessMask = 0;
			imb.dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
			srcStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
			dstStage = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
		}
		else if (newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL)
		{
			imb.srcAccessMask = 0;
			imb.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
			srcStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
			dstStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
		}
	}
	else if (oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL)
	{
		imb.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
		imb.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
		srcStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
		dstStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
	}

	vkCmdPipelineBarrier(commandBuffer, srcStage, dstStage, 0, 0, NULL, 0, NULL, 1, &imb);

	endSingleTimeCommands(&commandBuffer);
}
unsigned char assessError(VkResult result)
{
	if (result == 0)
		return 0;
	else if (result == -9)
	{
		printf("Unable to locate compatible driver, organism.\n");
		printf("It is recommended that GPU drivers are updated.\n");
		return 1;
	}
	else 
	{
		printf("%i\n", result);
		return 1;
	}
}
void prepareMathCache(textBox* cache, VkDescriptorPool* pool)
{

}
void prepareDescriptorSetLayouts()
{
	// image sampler
	createDescriptorSetLayout(0, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1, VK_SHADER_STAGE_FRAGMENT_BIT, &vkshit.setLayouts.imageSampler);
	// text box 2
	VkDescriptorSetLayoutBinding bindings[2];
	bindings[0].binding = 0;
	bindings[0].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	bindings[0].descriptorCount = 1;
	bindings[0].stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
	bindings[0].pImmutableSamplers = NULL;
	bindings[1].binding = 1;
	bindings[1].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	bindings[1].descriptorCount = 1;
	bindings[1].stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
	bindings[1].pImmutableSamplers = NULL;
	createDescriptorSetLayout2(bindings, 2, &vkshit.setLayouts.textBox2layout);
}
void preparePipelines()
{
	// Collection of custom data generation; content creation.
	// Ultimately put each unique into own function and create/destroy as necessary to optimize. i.e., for menu, prepare data for spinning cube and text boxes and once in game, destroy said data
	// with vkDestroyPipeline and vkDestroyPipelineLayout, all compacted into prepareMenuScene() and destroyMenuScene()

	// spinning cube
	VkVertexInputBindingDescription vibd;
	vibd.binding = 0;
	vibd.stride = sizeof(float) * 6;
	vibd.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
	VkVertexInputAttributeDescription viad[2];
	viad[0].location = 0;
	viad[0].binding = 0;
	viad[0].format = VK_FORMAT_R32G32B32_SFLOAT;
	viad[0].offset = 0;
	viad[1].location = 1;
	viad[1].binding = 0;
	viad[1].format = VK_FORMAT_R32G32B32_SFLOAT;
	viad[1].offset = sizeof(float) * 3;
	VkPipelineLayoutCreateInfo layoutInfo;
	// 16 push constants; view vector, projection vector, and rotation matrix
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

	vkCreatePipelineLayout(vkshit.device, &layoutInfo, NULL, &vkshit.pipelines.spinningCube.layout);
	createVFPipeline("shaders/spinningCubeVert.spv", "shaders/frag.spv", &vibd, 1, viad, 2, vkshit.pipelines.spinningCube.layout, &vkshit.pipelines.spinningCube.line);

	// text box
	VkVertexInputBindingDescription vibd2;
	vibd2.binding = 0;
	vibd2.stride = sizeof(float) * 4;
	vibd2.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
	VkVertexInputAttributeDescription viad2;
	viad2.location = 0;
	viad2.binding = 0;
	viad2.format = VK_FORMAT_R32G32B32A32_SFLOAT;
	viad2.offset = 0;
	VkPipelineLayoutCreateInfo li2;
	li2.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	li2.pNext = NULL;
	li2.flags = 0;
	li2.setLayoutCount = 1;
	li2.pSetLayouts = &vkshit.setLayouts.imageSampler;
	li2.pushConstantRangeCount = 0;
	li2.pPushConstantRanges = NULL;

	vkCreatePipelineLayout(vkshit.device, &li2, NULL, &vkshit.pipelines.textBox.layout);
	createVFPipeline("shaders/guiTextVert.spv", "shaders/guiTextFrag.spv", &vibd2, 1, &viad2, 1, vkshit.pipelines.textBox.layout, &vkshit.pipelines.textBox.line);

	// text box 2
	li2.pSetLayouts = &vkshit.setLayouts.textBox2layout;
	vkCreatePipelineLayout(vkshit.device, &li2, NULL, &vkshit.pipelines.textBox2.layout);
	createVFPipeline("shaders/guiTextVert.spv", "shaders/guiTextFrag2.spv", &vibd2, 1, &viad2, 1, vkshit.pipelines.textBox2.layout, &vkshit.pipelines.textBox2.line);

}
void destroyUBO(UBO* UBO)
{
	vkFreeMemory(vkshit.device, UBO->mem, NULL);
	vkDestroyBuffer(vkshit.device, UBO->buffer, NULL);
	UBO->size = 0;
	UBO->pool = NULL;
}
void destroyPipelines()
{
	vkDestroyPipeline(vkshit.device, vkshit.pipelines.spinningCube.line, NULL);
	vkDestroyPipelineLayout(vkshit.device, vkshit.pipelines.spinningCube.layout, NULL);
	vkDestroyPipeline(vkshit.device, vkshit.pipelines.textBox.line, NULL);
	vkDestroyPipelineLayout(vkshit.device, vkshit.pipelines.textBox.layout, NULL);
}
void destroyTextBox(textBox* box)
{
	vkDestroyImage(vkshit.device, box->image.data, NULL);
	vkDestroyImageView(vkshit.device, box->image.view, NULL);
	vkFreeMemory(vkshit.device, box->image.mem, NULL);
	vkDestroyBuffer(vkshit.device, box->buffer, NULL);
	vkFreeMemory(vkshit.device, box->memory, NULL);
	vkFreeDescriptorSets(vkshit.device, *box->pool, 1, &box->set);
}
void cleanVk()
{
	unsigned char i;
	
	printf("Cleaning up Vulkan...\n");
	printf("Destroying semaphores...\n");
	vkDestroySemaphore(vkshit.device, vkshit.iSemaphore, NULL);
	vkDestroySemaphore(vkshit.device, vkshit.fSemaphore, NULL);
	printf("Destroying command pool...\n");
	vkDestroyCommandPool(vkshit.device, vkshit.commandPool, NULL); // implicityly destroys VkCommandBuffer
	printf("Destroying framebuffers...\n");
	for (i = 0; i < vkshit.imageCount; i++)
	{
		vkDestroyFramebuffer(vkshit.device, vkshit.framebuffers[i], NULL);
	}
	printf("Destroying render pass...\n");
	vkDestroyRenderPass(vkshit.device, vkshit.renderPass, NULL);
	printf("Destroying images...\n");
	vkDestroyImage(vkshit.device, vkshit.depthImage, NULL);
	vkFreeMemory(vkshit.device, vkshit.depthImageMemory, NULL);
	printf("Destroying image views...\n");
	for (i = 0; i < vkshit.imageCount; i++)
	{
		vkDestroyImageView(vkshit.device, vkshit.imageViews[i], NULL);
	}
	printf("Image views destroyed.\n");
	printf("Destroying swap chain...\n");
	vkDestroySwapchainKHR(vkshit.device, vkshit.swapChain, NULL); // implicitly destroys VkImage
	printf("Swap chain destroyed.\n");
	printf("Destroying device...\n");
	vkDestroyDevice(vkshit.device, NULL);  // implicitly destroys VkQueue
	printf("Device destroyed.\n");
	printf("Destroying surface...\n");
	vkDestroySurfaceKHR(vkshit.instance, vkshit.surface, NULL);
	printf("Surface destroyed.\n");
	printf("Destroying instance...\n");
	vkDestroyInstance(vkshit.instance, NULL); // implicitly destroys VkPhysicalDevice
	printf("Instance destroyed.\n");

	printf("Vulkan clean up successful.\n");
}
