#pragma once
// WSI selection (better to create separate vk.o for each kernel and window library and build on each for new releases)
#define VK_USE_PLATFORM_WIN32_KHR
// #define VK_USE_PLATFORM_XLIB_KHR
// #define VK_USE_PLATFORM_WAYLAND_KHR
// #define VK_USE_PLATFORM_MIR_KHR

// standard
#include <stdio.h>
// external
#include <vulkan\vulkan.h>
// local
#include "sdl.h"
#include "maths.h"
#include "perception.h"

// type declarations
typedef struct {
	VkBuffer vertexBuffer;
	VkDeviceMemory memory;
} bufferShit;

typedef struct {
	VkBuffer buffer;
	VkDeviceMemory mem;
	VkDeviceSize size;
	VkDescriptorPool* pool;
} UBO;

typedef struct {
	size_t codeSize;
	uint32_t *code;
} shaderHandle;

typedef struct {
	float pos[2];
} vertex2D;

typedef struct {
	vec3 pos;
	vec3 color;
} vertex;

typedef struct {
	VkPipeline line;
	VkPipelineLayout layout;
} pipe;

typedef struct {
	VkImage data;
	VkDeviceMemory mem;
	VkImageView view;
} image;

typedef struct {
	image image;
	float quad[24];
	float textColor[3];
	float selectedColor[3];
	unsigned char selected;
	uint32_t RPixel;
	uint32_t LPixel;
	uint32_t TPixel;
	uint32_t BPixel;
	VkBuffer buffer; // vertex buffer
	VkDeviceMemory memory; // vertex buffer memory
	UBO textColorUBO;
	VkDescriptorSet set;
	VkDescriptorPool* pool;
} textBox;

// global handles
struct {
	VkInstance instance;
	VkSurfaceKHR surface;
	VkPhysicalDevice physicalDevice;
	uint32_t queueFamilyIndex;
	VkDevice device;
	VkQueue queue;
	uint32_t imageCount;
	VkFormat imageFormat;
	VkSwapchainKHR swapChain;
	VkImage images[2];
	VkImageView imageViews[2];
	VkRenderPass renderPass;
	VkFramebuffer framebuffers[2];
	VkCommandPool commandPool;
	VkSemaphore iSemaphore;
	VkSemaphore fSemaphore;
	VkSampler sampler;

	VkImage depthImage;
	VkDeviceMemory depthImageMemory;
	VkImageView depthImageView;

	VkPresentInfoKHR presentInfo;
	VkSubmitInfo submitInfo;
	
	struct {
		VkDescriptorSetLayout imageSampler;
		VkDescriptorSet imageSamplerSet;
		VkDescriptorSetLayout textBox2layout;
		VkDescriptorSet textBox2set;
	} setLayouts;
	struct {
		pipe spinningCube;
		pipe textBox;
		pipe textBox2;
	} pipelines;
	struct {
		bufferShit cubeVert;
		bufferShit cubeIndex;
	} primitives;
	struct {
		textBox number[10];
	} mathCache;
} vkshit;
/*
struct {

} primitives;

struct {

} commandBuffers;
*/
vertex triangle[3];
vertex square[4];
uint16_t squareIndices[6];
float cube[48];
uint16_t cubeIndices[36];
vertex pyramid[4];
uint16_t pyramidIndices[12];
vertex pentagon[6];
uint16_t pentagonIndices[15];

float rotationMatrix[9];

struct {
	vec3 pos[9];
} worldMesh;

// prototypes
// global state
unsigned char initVk();
unsigned char createInstance();
unsigned char createSurface();
unsigned char createPhysicalDevice();
unsigned char createDevice();
unsigned char createSwapChain();
unsigned char createImageViews();
unsigned char createRenderPass();
unsigned char createFramebuffers();
unsigned char createSemaphores();
unsigned char createSampler();
// abstractified
// pipelines
void createVFPipeline(char* vertexShader, char* fragmentShader, VkVertexInputBindingDescription* vibd, uint32_t vibdNum, VkVertexInputAttributeDescription* viad, uint32_t viadNum,
	VkPipelineLayout layout, VkPipeline* pipeline);
// descriptors
void createDescriptorPool(VkDescriptorPoolSize* poolSizes, uint32_t poolSizeCount, uint32_t maxSets, VkDescriptorPool* pool);
void createDescriptorSetLayout(uint32_t binding, VkDescriptorType type, uint32_t descriptorCount, VkShaderStageFlags stageFlags, VkDescriptorSetLayout* setLayout);
void createDescriptorSetLayout2(VkDescriptorSetLayoutBinding* bindings, uint32_t bindingCount, VkDescriptorSetLayout* layout);
void allocateDescriptorSet(VkDescriptorPool pool, VkDescriptorSetLayout* setLayout, VkDescriptorSet* set);
void updateDescriptorSetImage(uint32_t binding, VkSampler sampler, VkImageView imageView, VkDescriptorSet set);
void updateDescriptorSetUBO(uint32_t binding, UBO* UBO, VkDescriptorSet set);
// commands
void createCommandPool(VkCommandPoolCreateFlags flags, VkCommandPool* pool);
void allocateCommandBuffers(uint32_t count, VkCommandPool pool, VkCommandBufferLevel level, VkCommandBuffer* cmds);
void beginCommandBuffer(VkCommandBufferUsageFlags usage, VkCommandBuffer cmd);
// buffers
void createUniformBuffer(VkDeviceSize size, UBO* UBO);
void updateUniformBuffer(void* data, UBO* UBO);
// render pass
void beginRenderPass(VkCommandBuffer cmd, VkRenderPass renderPass, VkFramebuffer framebuffer, VkClearValue clearColor);
// drawing
void prepareDrawInfo();
void draw(VkCommandBuffer* cmds);
// resize
void resizeVkPrepare();
void resizeVkDestroy();

unsigned char createDepthResources();
void destroyDepthResources();
void loadShader(shaderHandle* handle, char* file);
unsigned char createVertexBuffer(float* vertices, VkDeviceSize size, VkBuffer* buffer, VkDeviceMemory* bufferMemory);
unsigned char createMeshBuffer(vec3* mesh, VkDeviceSize size, VkBuffer* buffer, VkDeviceMemory* bufferMemory);
unsigned char createIndexBuffer(uint16_t* indices, VkDeviceSize size, VkBuffer* buffer, VkDeviceMemory* bufferMemory);
unsigned char createImageView(VkImage image, VkFormat format, VkImageAspectFlags aspectMask, VkImageView* imageView);
unsigned char createImage(uint32_t width, uint32_t height, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties, VkImage* image, VkDeviceMemory* imageMemory);
unsigned char createBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer* buffer, VkDeviceMemory* bufferMemory);
unsigned char copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);
void createTextBox(char* text, float xpos, float ypos, SDL_Color color, textBox* box, VkDescriptorPool* pool);
void copyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height);
void beginSingleTimeCommands(VkCommandBuffer* commandBuffer);
void endSingleTimeCommands(VkCommandBuffer* commandBuffer);
void transitionImageLayout(VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout);
unsigned char assessError(VkResult result);
// content init
void initGeometry();
void prepareMathCache(textBox* cache, VkDescriptorPool* pool);
void prepareDescriptorSetLayouts();
void preparePipelines();
// clean up
void destroyUBO(UBO* UBO);
void destroyPipelines();
void destroyTextBox(textBox* box);
void cleanVk();