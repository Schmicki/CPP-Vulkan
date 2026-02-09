#pragma once

#include "Types/Types.h"
#include "Interfaces/RenderInterface.h"
#include "Window/Window.h"

#if SGE_PLATFORM_WINDOWS // windows only

#define VK_USE_PLATFORM_WIN32_KHR
#include <Windows.h>

#elif SGE_PLATFORM_LINUX // linux only

#define VK_USE_PLATFORM_XCB_KHR

#endif

#include "vulkan/vulkan.h"

#if SGE_PLATFORM_WINDOWS // windows only
	#define VULKAN_WINDOW_SURFACE_NAME VK_KHR_WIN32_SURFACE_EXTENSION_NAME
#elif SGE_PLATFORM_LINUX // linux only
	#define VULKAN_WINDOW_SURFACE_NAME VK_KHR_XCB_SURFACE_EXTENSION_NAME
#else
	#define VULKAN_WINDOW_SURFACE_NAME ""
#endif

namespace sge {

	struct UniformBufferObject {
		sge::FMat4 model;
		sge::FMat4 view;
		sge::FMat4 proj;
	};

	VkVertexInputBindingDescription* getBindingDescription();
	VkVertexInputAttributeDescription* getAttributeDescriptions();

	struct VksBuffer {
		VkBuffer handle;
		VkDeviceMemory mem;
	};

	struct VksImage {
		VkImage handle;
		VkDeviceMemory mem;
		VkImageView view;
	};

	struct VksSampledImage {
		VkImage handle;
		VkDeviceMemory memory;
		VkImageView view;
		VkSampler sampler;
	};

	struct VksCommandPool {
		VkCommandPool handle;
		VkQueue queue;
		u32 family;
	};

	class VulkanRenderInterface : public IRenderInterface {
	public:
		virtual ~VulkanRenderInterface(){};
		virtual bool initialize(sge::Window &window) override;
		virtual void onWindowResized() override;
		virtual void drawFrame() override;
		virtual void destroy() override;

		void getPhysicalDevice(bool best);
		u32 getQueueFamily(VkQueueFlags flags);
		void getSurfaceFormat();
		void getPresentMode();
		u32 findMemType(VkMemoryRequirements requirements, VkMemoryPropertyFlags properties);
		VkFormat containsFormatWithFeatures(const Array<VkFormat>& formats, u32 tiling, VkFormatFeatureFlags featuers);
		void createCmdPool(u32 queueFamily);
		VkCommandBuffer allocCmdBuffer(u32 level);
		VkCommandBuffer* allocCmdBuffers(u32 count, u32 level);
		VkCommandBuffer beginTinyCommands();
		void endTinyCommands(VkCommandBuffer buffer);
		VksBuffer createBuffer(u32 size, VkBufferUsageFlags usage, VkMemoryPropertyFlags props, VkSharingMode sharingMode);
		VksBuffer* createBuffers(u32 count, u32 size, VkBufferUsageFlags usage, VkMemoryPropertyFlags props, VkSharingMode sharingMode);
		VksBuffer createStagingBuffer(void* data, u32 size);
		void freeBuffer(VksBuffer buffer);
		void copyBuffer(VkBuffer src, VkBuffer dst, u32 size);
		void copyBufferToImage(VkBuffer src, VkImage dst, u32 width, u32 height);
		VksBuffer createIndexBuffer(const Array<u32>& indices);
		VksBuffer createVertexBuffer(const Array<Vertex>& vertices);
		void changeImgLayoutWithMipsAndAspect(VkImage img, VkImageLayout old, VkImageLayout _new, u32 format, u32 aspect, u32 mips, u32 baseMip);
		void changeColoredImgLayout(VkImage img, VkImageLayout old, VkImageLayout _new, u32 format);
		void genMipMaps(VkImage image, VkFormat format, u32 width, u32 height, u32 num);
		VksImage createSampledImageWithMips(u32 width, u32 height, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkSampleCountFlags samples, u32 mipLevels);
		VksImage createImage(u32 width, u32 height, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage);
		VksImage createSampledImage(u32 width, u32 height, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkSampleCountFlags samples);
		VksImage createImageWithMips(u32 width, u32 height, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, u32 mips);
		void fillImage(VkImage img, u32 width, u32 height, Color* pixels, u32 format);
		void fillImageWithMips(VkImage img, u32 width, u32 height, Color* pixels, VkFormat format, u32 mips);
		VkImageView createImageViewWithMipAndType(VkImage img, VkFormat format, VkImageViewType viewType, u32 aspect, u32 baseMip, u32 mips);
		VkImageView createImageView2DWithMip(VkImage img, VkFormat format, u32 aspect, u32 baseMip, u32 mips);
		VkImageView createImageView2D(VkImage img, VkFormat format, u32 aspect);
		VkSampler createSamplerWithMips(u32 mips, VkFilter filter, VkSamplerAddressMode uvmode);
		VkSampler createSamplerLinearRepeatWithMips(u32 mips);
		VkSampler createSamplerLinearRepeat();
		VkShaderModule createShaderModule(Buffer* code);

		bool startFrame();
		void endFrame();

		void prepareStuff();
		void destroyStuff();

		void prepareFrameDependantStuff();
		void destroyFrameDependantStuff();
		void updateFrameDependantStuff();

		// Stuff
		VksSampledImage texture;
		VksBuffer vertexBuffer;
		VksBuffer indexBuffer;
		u32 indexCount;
		VkDescriptorSetLayout cubeDSL;
		VkDescriptorPool cubeDP;
		VkPipelineLayout pipelineLayout;
		VkPipeline pipeline;

		// Per frame stuff
		VksBuffer* uniformBuffers;
		VkDescriptorSet *descriptorSets;

		VkInstance instance;
		VkPhysicalDevice gpu;
		VkDevice device;
		VkPhysicalDeviceFeatures enabledFeatures;
		VkSampleCountFlags msaa;
		VksCommandPool cmdPool;

		// Surface
		VkSurfaceKHR surface;
		u32 width;
		u32 height;
		VkPresentModeKHR presentMode;
		VkSurfaceFormatKHR format;
		VkFormat depthFormat;
		u32 minImgCount;
		u32 maxImgCount;
		VkSurfaceTransformFlagsKHR transform;
		VkRenderPass renderPass;

		// Swapchain
		VkSwapchainKHR swapchain;
		u32 imgCount;
		VkImage *imgs;
		VkImageView *views;
		VksImage colImg;
		VksImage depthImg;
		bool windowResized;
		VkFramebuffer *framebuffers;
		
		// Synchronization objects
		u32 parallelFrameCount;
		VkCommandBuffer* CmdBuffers;
		VkSemaphore* gotImgSemaphores;
		VkSemaphore* doneSemaphores;
		VkFence* renderFences;
		u32* frameMap;
		VkCommandBuffer drawCmdBuffer;
		u32 syncFrame;
		u32 currentFrame;
		u32 currentImg;
	};
}