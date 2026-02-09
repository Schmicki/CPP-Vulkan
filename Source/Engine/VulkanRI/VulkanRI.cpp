#include "VulkanRI.h"

#include <iostream>
#include <stdexcept>
#include <chrono>

static u32 res = 0;
#define VKR_CHECK(result, message)                              									\
	res = result;                                               									\
	if (res != VK_SUCCESS) {                                    									\
		std::cout << message << " error: " << res << ", " << __FILE__ << " : " << __LINE__ << std::endl; 	\
		throw std::runtime_error(message);                      									\
	}

#define VKST(type) VK_STRUCTURE_TYPE_##type
#define VKCI(type) VK_STRUCTURE_TYPE_##type##_CREATE_INFO
#define VKCIK(type) VK_STRUCTURE_TYPE_##type##_CREATE_INFO_KHR
#define VKBU(buffUsage) VK_BUFFER_USAGE_##buffUsage##_BIT
#define VKIU(imgUsage) VK_IMAGE_USAGE_##imgUsage##_BIT
#define VKMP(prop) VK_MEMORY_PROPERTY_##prop##_BIT

namespace sge {

	VkVertexInputBindingDescription* getBindingDescription() {
		return new VkVertexInputBindingDescription[1]{
			{0, sizeof(sge::Vertex), VK_VERTEX_INPUT_RATE_VERTEX }
		};
	}

	VkVertexInputAttributeDescription* getAttributeDescriptions() {
		return new VkVertexInputAttributeDescription[3]{
			{0, 0, VK_FORMAT_R32G32B32_SFLOAT, 0},
			{1, 0, VK_FORMAT_R32G32B32_SFLOAT, sizeof(sge::FVec3)},
			{2, 0, VK_FORMAT_R32G32_SFLOAT, sizeof(sge::FVec3) * 2}
		};
	}

	void VulkanRenderInterface::prepareStuff() {
		Model* model = sge::loadModel("/Models/violin.sad");
		if (!model) return;

		indexCount = model->indices.size;
		indexBuffer = createIndexBuffer(model->indices);
		vertexBuffer = createVertexBuffer(model->vertices);
		delete model;

		Texture* tex = loadTexture("/Textures/violin.png");
		if (!tex) return;

		u32 mips = 1 + log2(max(tex->height, tex->width));
		*reinterpret_cast<VksImage*>(&texture) = createImageWithMips(
			tex->width, tex->height, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_TILING_OPTIMAL,
			VKIU(TRANSFER_SRC) | VKIU(SAMPLED) | VKIU(TRANSFER_DST), mips
		);
		fillImageWithMips(texture.handle, tex->width, tex->height, tex->pixels, VK_FORMAT_R8G8B8A8_SRGB, mips);
		texture.view = createImageView2DWithMip(texture.handle, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_ASPECT_COLOR_BIT, 0, mips);
		texture.sampler = createSamplerLinearRepeatWithMips(mips);
		delete tex;

		VkDescriptorSetLayoutBinding bindings[] = {
			{0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1, VK_SHADER_STAGE_VERTEX_BIT, NULL},
			{1, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1, VK_SHADER_STAGE_FRAGMENT_BIT, NULL}
		};

		VkDescriptorSetLayoutCreateInfo dslInfo;
		dslInfo.sType = VKCI(DESCRIPTOR_SET_LAYOUT);
		dslInfo.pNext = NULL;
		dslInfo.flags = 0;
		dslInfo.bindingCount = 2;
		dslInfo.pBindings = bindings;
		vkCreateDescriptorSetLayout(device, &dslInfo, NULL, &cubeDSL);

		Buffer* vertShader = loadFileAligned4("/Shaders/defaultVertShader.vert.spv");
		Buffer* fragShader = loadFileAligned4("/Shaders/defaultFragShader.frag.spv");

		VkPipelineShaderStageCreateInfo stages[2] = {
			{VKCI(PIPELINE_SHADER_STAGE), NULL, 0, VK_SHADER_STAGE_VERTEX_BIT, createShaderModule(vertShader), "main", NULL},
			{VKCI(PIPELINE_SHADER_STAGE), NULL, 0, VK_SHADER_STAGE_FRAGMENT_BIT, createShaderModule(fragShader), "main", NULL}
		};

		VkPipelineVertexInputStateCreateInfo vertexInputState{VKCI(PIPELINE_VERTEX_INPUT_STATE), NULL, 0, 1, getBindingDescription(), 3, getAttributeDescriptions()};
		VkPipelineInputAssemblyStateCreateInfo inputAssemblyState{VKCI(PIPELINE_INPUT_ASSEMBLY_STATE), NULL, 0, VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST, VK_FALSE};
		VkPipelineViewportStateCreateInfo viewportState{VKCI(PIPELINE_VIEWPORT_STATE), NULL, 0, 1, NULL, 1, NULL};
		VkPipelineRasterizationStateCreateInfo rasterizationState{VKCI(PIPELINE_RASTERIZATION_STATE), NULL, 0, VK_FALSE, VK_FALSE, VK_POLYGON_MODE_FILL, VK_CULL_MODE_NONE, VK_FRONT_FACE_CLOCKWISE, VK_FALSE, 0.0F, 0.0F, 0.0F, 1.0F};
		VkPipelineMultisampleStateCreateInfo multisampleState{VKCI(PIPELINE_MULTISAMPLE_STATE), NULL, 0, static_cast<VkSampleCountFlagBits>(msaa), VK_FALSE, 1.0F, NULL, VK_FALSE, VK_FALSE};
		VkPipelineDepthStencilStateCreateInfo depthStencilState{VKCI(PIPELINE_DEPTH_STENCIL_STATE), NULL, 0, VK_TRUE, VK_TRUE, VK_COMPARE_OP_GREATER, VK_FALSE, VK_FALSE, {}, {}, 0.0F, 1.0F};
		VkPipelineColorBlendAttachmentState colorBlendAttachment{VK_TRUE, VK_BLEND_FACTOR_SRC_ALPHA, VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA, VK_BLEND_OP_ADD, VK_BLEND_FACTOR_ONE, VK_BLEND_FACTOR_ZERO, VK_BLEND_OP_ADD, VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT};
		VkPipelineColorBlendStateCreateInfo colorBlendState{VKCI(PIPELINE_COLOR_BLEND_STATE), NULL, 0, VK_FALSE, VK_LOGIC_OP_COPY, 1, &colorBlendAttachment, {0.0F, 0.0F, 0.0F, 0.0F}};
		VkDynamicState dynamicStates[2] = {VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR};
		VkPipelineDynamicStateCreateInfo dynamicState{VKCI(PIPELINE_DYNAMIC_STATE), NULL, 0, 2, dynamicStates};

		VkPipelineLayoutCreateInfo layoutInfo {VKCI(PIPELINE_LAYOUT), NULL, 0, 1, &cubeDSL, 0, NULL};
		vkCreatePipelineLayout(device, &layoutInfo, NULL, &pipelineLayout);
		
		VkGraphicsPipelineCreateInfo cubePipeInfo;
		cubePipeInfo.sType = VKCI(GRAPHICS_PIPELINE);
		cubePipeInfo.pNext = NULL;
		cubePipeInfo.flags = 0;
		cubePipeInfo.stageCount = 2;
		cubePipeInfo.pStages = stages;
		cubePipeInfo.pVertexInputState = &vertexInputState;
		cubePipeInfo.pInputAssemblyState = &inputAssemblyState;
		cubePipeInfo.pTessellationState = NULL;
		cubePipeInfo.pViewportState = &viewportState;
		cubePipeInfo.pRasterizationState = &rasterizationState;
		cubePipeInfo.pMultisampleState = &multisampleState;
		cubePipeInfo.pDepthStencilState = &depthStencilState;
		cubePipeInfo.pColorBlendState = &colorBlendState;
		cubePipeInfo.pDynamicState = &dynamicState;
		cubePipeInfo.layout = pipelineLayout;
		cubePipeInfo.renderPass = renderPass;
		cubePipeInfo.subpass = 0;
		cubePipeInfo.basePipelineHandle = VK_NULL_HANDLE;
		cubePipeInfo.basePipelineIndex = -1;
		vkCreateGraphicsPipelines(device, NULL, 1, &cubePipeInfo, NULL, &pipeline);

		delete[] vertexInputState.pVertexBindingDescriptions;
		delete[] vertexInputState.pVertexAttributeDescriptions;
		vkDestroyShaderModule(device, stages[0].module, NULL);
		vkDestroyShaderModule(device, stages[1].module, NULL);
		delete vertShader;
		delete fragShader;
	}

	void VulkanRenderInterface::destroyStuff() {
		vkDestroyPipeline(device, pipeline, NULL);
		vkDestroyPipelineLayout(device, pipelineLayout, NULL);
		vkDestroyDescriptorSetLayout(device, cubeDSL, NULL);
		vkDestroySampler(device, texture.sampler, NULL);
		vkDestroyImageView(device, texture.view, NULL);
		vkFreeMemory(device, texture.memory, NULL);
		vkDestroyImage(device, texture.handle, NULL);

		freeBuffer(vertexBuffer);
		freeBuffer(indexBuffer);
	}

	void VulkanRenderInterface::prepareFrameDependantStuff() {
		uniformBuffers = createBuffers(parallelFrameCount, sizeof(UniformBufferObject), VKBU(UNIFORM_BUFFER), VKMP(HOST_COHERENT) | VKMP(HOST_VISIBLE), VK_SHARING_MODE_EXCLUSIVE);
		VkDescriptorPoolSize poolSizes[] = {
			{VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, parallelFrameCount},
			{VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, parallelFrameCount}
		};
		VkDescriptorPoolCreateInfo descriptorPoolInfo;
		descriptorPoolInfo.sType = VKCI(DESCRIPTOR_POOL);
		descriptorPoolInfo.pNext = NULL;
		descriptorPoolInfo.flags = 0;
		descriptorPoolInfo.maxSets = parallelFrameCount;
		descriptorPoolInfo.poolSizeCount = 2;
		descriptorPoolInfo.pPoolSizes = poolSizes;
		vkCreateDescriptorPool(device, &descriptorPoolInfo, NULL, &cubeDP);

		VkDescriptorSetLayout* layouts = new VkDescriptorSetLayout[parallelFrameCount];
		for (u32 i = 0; i < parallelFrameCount; i++) {
			layouts[i] = cubeDSL;
		}


		VkDescriptorSetAllocateInfo descriptorAllocInfo{VKST(DESCRIPTOR_SET_ALLOCATE_INFO), NULL, cubeDP, parallelFrameCount, layouts};
		descriptorSets = new VkDescriptorSet[parallelFrameCount];
		vkAllocateDescriptorSets(device, &descriptorAllocInfo, descriptorSets);

		for (u32 i = 0; i < parallelFrameCount; i++) {
			VkDescriptorBufferInfo descriptorUniformBufferInfo{uniformBuffers[i].handle, 0, sizeof(UniformBufferObject)};
			VkDescriptorImageInfo descriptorImageInfo{texture.sampler, texture.view, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL};
			VkWriteDescriptorSet writes[] = {
				{VKST(WRITE_DESCRIPTOR_SET), NULL, descriptorSets[i], 0, 0, 1, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, NULL, &descriptorUniformBufferInfo, NULL},
				{VKST(WRITE_DESCRIPTOR_SET), NULL, descriptorSets[i], 1, 0, 1, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, &descriptorImageInfo, NULL, NULL}
			};
			vkUpdateDescriptorSets(device, 2, writes, 0, NULL);
		}
	}

	void VulkanRenderInterface::destroyFrameDependantStuff() {
		for (u32 i = 0; i < parallelFrameCount; i++) {
			freeBuffer(uniformBuffers[i]);
		}
		delete[] uniformBuffers;
		delete[] descriptorSets;
		vkDestroyDescriptorPool(device, cubeDP, NULL);
	}

	void VulkanRenderInterface::updateFrameDependantStuff() {
		static std::chrono::steady_clock::time_point startTime = std::chrono::steady_clock::now();

		f32 time = std::chrono::duration<f32, std::chrono::seconds::period>(startTime - std::chrono::steady_clock::now()).count();

		UniformBufferObject ubo;
		ubo.model = sge::FMat4::transposed(sge::FMat4::transformation({ 0.0f, 2.0f, 0.0f }, sge::FQuat::quat({ 0.0f, 0.0f, 1.0f }, time * 45.0f), { 1, 1, 1 }));
		ubo.view = sge::FMat4::transposed(sge::FMat4::identity(1.0f).translate(-sge::FVec3{ 0.0f, 0.0f, 0.0f }).rotate(sge::FQuat::quat({ 0, 0, 1 }, 0.0f)));
		ubo.proj = sge::FMat4::transposed(sge::FMat4::perspectiveIRZ(90.0f, FVec2{to<float>(width), to<float>(height)}, 0.1f));

		void* data;
		vkMapMemory(device, uniformBuffers[currentFrame].mem, 0, sizeof(UniformBufferObject), 0, &data);
		memcpy(data, &ubo, sizeof(UniformBufferObject));
		vkUnmapMemory(device, uniformBuffers[currentFrame].mem);
	}

	void VulkanRenderInterface::drawFrame() {
		if (!startFrame()) return;

		vkCmdBindPipeline(drawCmdBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline);
		vkCmdBindDescriptorSets(drawCmdBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, 0, 1, &descriptorSets[currentFrame], 0, NULL);

		VkBuffer vertexBuffers[] = {vertexBuffer.handle};
		VkDeviceSize offsets[] = {0};
		vkCmdBindVertexBuffers(drawCmdBuffer, 0, 1, vertexBuffers, offsets);
		vkCmdBindIndexBuffer(drawCmdBuffer, indexBuffer.handle, 0, VK_INDEX_TYPE_UINT32);
		vkCmdDrawIndexed(drawCmdBuffer, indexCount, 1, 0, 0, 0);

		endFrame();
	}

	bool formatHasStencilComponent(u32 format) {
		return (format == VK_FORMAT_D32_SFLOAT_S8_UINT || format == VK_FORMAT_D24_UNORM_S8_UINT || format == VK_FORMAT_D16_UNORM_S8_UINT);
	}

	void VulkanRenderInterface::getPhysicalDevice(bool best) {
		u32 num = 0;
		vkEnumeratePhysicalDevices(instance, &num, NULL);
		VkPhysicalDevice* phys = new VkPhysicalDevice[num];
		vkEnumeratePhysicalDevices(instance, &num, phys);
		if (num == 0) {
			VKR_CHECK(VK_ERROR_UNKNOWN, "failed to find physical device!");
		}
		else if (num == 1) {
			gpu = phys[0];
		}
		else {
			gpu = phys[0];
		}
		delete[] phys;
	}

	u32 VulkanRenderInterface::getQueueFamily(VkQueueFlags flags) {
		u32 num = 0;
		vkGetPhysicalDeviceQueueFamilyProperties(gpu, &num, NULL);
		SelfDeletingPointer<VkQueueFamilyProperties> props = new VkQueueFamilyProperties[num];
		vkGetPhysicalDeviceQueueFamilyProperties(gpu, &num, props.data);

		for (u32 i = 0; i < num; i++) {
			if ((props[i].queueFlags & flags) == flags) return i;
		}
		return UINT32_MAX;
	}

	void VulkanRenderInterface::getSurfaceFormat() {
		u32 num = 0;
		vkGetPhysicalDeviceSurfaceFormatsKHR(gpu, surface, &num, NULL);
		SelfDeletingPointer<VkSurfaceFormatKHR> formats = new VkSurfaceFormatKHR[num];
		vkGetPhysicalDeviceSurfaceFormatsKHR(gpu, surface, &num, formats.data);

		for (u32 i = 0; i < num; i++) {
			if (formats[i].format == VK_FORMAT_B8G8R8A8_UNORM && formats[i].colorSpace == VK_COLORSPACE_SRGB_NONLINEAR_KHR) {
				format = formats[i];
				return;
			}
		}
		for (u32 i = 0; i < num; i++) {
			if (formats[i].format == VK_FORMAT_R8G8B8A8_UNORM && formats[i].colorSpace == VK_COLORSPACE_SRGB_NONLINEAR_KHR) {
				format = formats[i];
				return;
			}
		}

		VKR_CHECK(VK_ERROR_UNKNOWN, "no compatible format found!");
		format = { VK_FORMAT_UNDEFINED, VK_COLORSPACE_SRGB_NONLINEAR_KHR };
	}

	void VulkanRenderInterface::getPresentMode() {
		u32 num;
		vkGetPhysicalDeviceSurfacePresentModesKHR(gpu, surface, &num, NULL);
		SelfDeletingPointer<VkPresentModeKHR> modes = new VkPresentModeKHR[num];
		vkGetPhysicalDeviceSurfacePresentModesKHR(gpu, surface, &num, modes.data);

		for (u32 i = 0; i < num; i++) {
			if (modes[i] == VK_PRESENT_MODE_IMMEDIATE_KHR) {
				presentMode = VK_PRESENT_MODE_IMMEDIATE_KHR;
				return;
			}
		}
		presentMode = VK_PRESENT_MODE_FIFO_KHR;
	}

	u32 VulkanRenderInterface::findMemType(VkMemoryRequirements reqs, VkMemoryPropertyFlags props){
		VkPhysicalDeviceMemoryProperties memProps;
		vkGetPhysicalDeviceMemoryProperties(gpu, &memProps);
		uint memTypeIdx = VK_MEMORY_PROPERTY_FLAG_BITS_MAX_ENUM;

		for (uint i = 0; i < memProps.memoryTypeCount; i++) {
			if ((reqs.memoryTypeBits & (1 << i)) != 0) {
				if ((memProps.memoryTypes[i].propertyFlags & props) == props) {
					memTypeIdx = i;
					break;
				}
			}
		}

		if (memTypeIdx == VK_MEMORY_PROPERTY_FLAG_BITS_MAX_ENUM) return VK_MEMORY_PROPERTY_FLAG_BITS_MAX_ENUM;
		else return memTypeIdx;
	}

	VkFormat VulkanRenderInterface::containsFormatWithFeatures(const Array<VkFormat>& formats, u32 tiling, VkFormatFeatureFlags features) {
		for (u32 i = 0; i < formats.size; i++) {
			VkFormatProperties formatProps;
			vkGetPhysicalDeviceFormatProperties(gpu, formats.get(i), &formatProps);
			if (tiling == VK_IMAGE_TILING_LINEAR && (formatProps.linearTilingFeatures & features) == features) {
				return formats.get(i);
			}
			else if (tiling == VK_IMAGE_TILING_OPTIMAL && (formatProps.optimalTilingFeatures & features) == features) {
				return formats.get(i);
			}
		}
		return VK_FORMAT_UNDEFINED;
	}

	void VulkanRenderInterface::createCmdPool(u32 queueFamily) {
		cmdPool.family = queueFamily;
		vkGetDeviceQueue(device, queueFamily, 0, &cmdPool.queue);
		VkCommandPoolCreateInfo createInfo {VKST(COMMAND_POOL_CREATE_INFO), NULL, VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT, queueFamily};
		VKR_CHECK(vkCreateCommandPool(device, &createInfo, NULL, &cmdPool.handle), "failed to create command pool!");
	}

	VkCommandBuffer VulkanRenderInterface::allocCmdBuffer(u32 level) {
		VkCommandBufferAllocateInfo info { VKST(COMMAND_BUFFER_ALLOCATE_INFO),  NULL, cmdPool.handle, static_cast<VkCommandBufferLevel>(level), 1 };
		VkCommandBuffer cmdBuff;
		VKR_CHECK(vkAllocateCommandBuffers(device, &info, &cmdBuff), "failed to create command buffer!");
		return cmdBuff;
	}

	VkCommandBuffer* VulkanRenderInterface::allocCmdBuffers(u32 count, u32 level) {
		VkCommandBufferAllocateInfo info { VKST(COMMAND_BUFFER_ALLOCATE_INFO),  NULL, cmdPool.handle, static_cast<VkCommandBufferLevel>(level), count };
		VkCommandBuffer* cmdBuffs = new VkCommandBuffer[count];
		VKR_CHECK(vkAllocateCommandBuffers(device, &info, cmdBuffs), "failed to create command buffer!");
		return cmdBuffs;
	}

	VkCommandBuffer VulkanRenderInterface::beginTinyCommands() {
		VkCommandBufferAllocateInfo allocInfo{};
		allocInfo.sType = VKST(COMMAND_BUFFER_ALLOCATE_INFO);
		allocInfo.pNext = NULL;
		allocInfo.commandPool = cmdPool.handle;
		allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		allocInfo.commandBufferCount = 1;

		VkCommandBuffer cmdBuffer;
		vkAllocateCommandBuffers(device, &allocInfo, &cmdBuffer);

		VkCommandBufferBeginInfo beginInfo{};
		beginInfo.sType = VKST(COMMAND_BUFFER_BEGIN_INFO);
		beginInfo.pNext = NULL;
		beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
		beginInfo.pInheritanceInfo = NULL;

		vkBeginCommandBuffer(cmdBuffer, &beginInfo);
		return cmdBuffer;
	}

	void VulkanRenderInterface::endTinyCommands(VkCommandBuffer buffer) {
		vkEndCommandBuffer(buffer);

		VkSubmitInfo submitInfo{};
		submitInfo.sType = VKST(SUBMIT_INFO);
		submitInfo.pNext = NULL;
		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &buffer;

		vkQueueSubmit(cmdPool.queue, 1, &submitInfo, VK_NULL_HANDLE);
		vkQueueWaitIdle(cmdPool.queue);

		vkFreeCommandBuffers(device, cmdPool.handle, 1, &buffer);
	}

	VksBuffer VulkanRenderInterface::createBuffer(u32 size, VkBufferUsageFlags usage, VkMemoryPropertyFlags props, VkSharingMode sharingMode) {
		VkBufferCreateInfo bufferCI{};
		bufferCI.sType = VKCI(BUFFER);
		bufferCI.pNext = NULL;
		bufferCI.flags = 0;
		bufferCI.size = size;
		bufferCI.usage = usage;
		bufferCI.sharingMode = sharingMode;

		VksBuffer buffer;
		VKR_CHECK(vkCreateBuffer(device, &bufferCI, NULL, &buffer.handle), "failed to create buffer!");

		VkMemoryRequirements memRequirements{};
		vkGetBufferMemoryRequirements(device, buffer.handle, &memRequirements);
		uint memTypeIdx = findMemType(memRequirements, props);
		if (memTypeIdx == VK_MEMORY_PROPERTY_FLAG_BITS_MAX_ENUM)
			VKR_CHECK(VK_ERROR_INITIALIZATION_FAILED, "buffer creation: failed to find compatible memory type!");

		VkMemoryAllocateInfo memAllocInfo{};
		memAllocInfo.sType = VKST(MEMORY_ALLOCATE_INFO);
		memAllocInfo.pNext = NULL;
		memAllocInfo.allocationSize = memRequirements.size;
		memAllocInfo.memoryTypeIndex = memTypeIdx;

		VKR_CHECK(vkAllocateMemory(device, &memAllocInfo, NULL, &buffer.mem), "failed to allocate buffer memory!");
		vkBindBufferMemory(device, buffer.handle, buffer.mem, 0);
		return buffer;
	}

	VksBuffer* VulkanRenderInterface::createBuffers(u32 count, u32 size, VkBufferUsageFlags usage, VkMemoryPropertyFlags props, VkSharingMode sharingMode) {
		VksBuffer* buffers = new VksBuffer[count];
		for (u32 i = 0; i < count; i++){
			buffers[i] = createBuffer(size, usage, props, sharingMode);
		}
		return buffers;
	}

	VksBuffer VulkanRenderInterface::createStagingBuffer(void* data, u32 size) {
		VkBufferUsageFlags usage = VKBU(TRANSFER_SRC);
		VkMemoryPropertyFlags props = VKMP(HOST_COHERENT) | VKMP(HOST_VISIBLE);

		VksBuffer buffer = createBuffer(size, usage, props, VK_SHARING_MODE_EXCLUSIVE);
		void* dst;
		vkMapMemory(device, buffer.mem, 0, size, 0, &dst);
		memcpy(dst, data, size);
		vkUnmapMemory(device, buffer.mem);
		return buffer;
	}

	void VulkanRenderInterface::freeBuffer(VksBuffer buffer) {
		vkDestroyBuffer(device, buffer.handle, NULL);
		vkFreeMemory(device, buffer.mem, NULL);
	}

	void VulkanRenderInterface::copyBuffer(VkBuffer src, VkBuffer dst, u32 size) {
		VkCommandBuffer cmds = beginTinyCommands();

		VkBufferCopy copyRegion{};
		copyRegion.size = size;
		copyRegion.srcOffset = 0;
		copyRegion.dstOffset = 0;
		vkCmdCopyBuffer(cmds, src, dst, 1, &copyRegion);

		endTinyCommands(cmds);
	}

	void VulkanRenderInterface::copyBufferToImage(VkBuffer src, VkImage dst, u32 width, u32 height) {
		VkCommandBuffer cmds = beginTinyCommands();

		VkBufferImageCopy buffToImg{};
		buffToImg.bufferOffset = 0;
		buffToImg.bufferRowLength = 0;
		buffToImg.bufferImageHeight = 0;
		buffToImg.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		buffToImg.imageSubresource.mipLevel = 0;
		buffToImg.imageSubresource.baseArrayLayer = 0;
		buffToImg.imageSubresource.layerCount = 1;
		buffToImg.imageOffset = { 0, 0, 0 };
		buffToImg.imageExtent = { width, height, 1 };

		vkCmdCopyBufferToImage(cmds, src, dst, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &buffToImg);
		endTinyCommands(cmds);
	}

	VksBuffer VulkanRenderInterface::createIndexBuffer(const Array<u32>& indices) {
		u32 size = sizeof(u32) * indices.size;
		VksBuffer stage = createStagingBuffer(indices.data, size);
		VksBuffer buffer = createBuffer(size, VKBU(INDEX_BUFFER) | VKBU(TRANSFER_DST), VKMP(DEVICE_LOCAL), VK_SHARING_MODE_EXCLUSIVE);
		copyBuffer(stage.handle, buffer.handle, size);
		freeBuffer(stage);
		return buffer;
	}

	VksBuffer VulkanRenderInterface::createVertexBuffer(const Array<Vertex>& vertices) {
		u32 size = sizeof(Vertex) * vertices.size;
		VksBuffer stage = createStagingBuffer(vertices.data, size);
		VksBuffer buffer = createBuffer(size, VKBU(VERTEX_BUFFER) | VKBU(TRANSFER_DST), VKMP(DEVICE_LOCAL), VK_SHARING_MODE_EXCLUSIVE);
		copyBuffer(stage.handle, buffer.handle, size);
		freeBuffer(stage);
		return buffer;
	}

	void VulkanRenderInterface::changeImgLayoutWithMipsAndAspect(VkImage img, VkImageLayout old, VkImageLayout _new, u32 format, u32 aspect, u32 mips, u32 baseMip) {
		VkCommandBuffer buff = beginTinyCommands();

		VkImageMemoryBarrier imgMemBarrier{};
		imgMemBarrier.sType = VKST(IMAGE_MEMORY_BARRIER);
		imgMemBarrier.pNext = NULL;
		imgMemBarrier.oldLayout = old;
		imgMemBarrier.newLayout = _new;
		imgMemBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		imgMemBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		imgMemBarrier.image = img;
		imgMemBarrier.subresourceRange.aspectMask = aspect;
		imgMemBarrier.subresourceRange.baseMipLevel = baseMip;
		imgMemBarrier.subresourceRange.levelCount = mips;
		imgMemBarrier.subresourceRange.baseArrayLayer = 0;
		imgMemBarrier.subresourceRange.layerCount = 1;

		if (_new == VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL && formatHasStencilComponent(format))
			imgMemBarrier.subresourceRange.aspectMask |= VK_IMAGE_ASPECT_STENCIL_BIT;

		VkPipelineStageFlags srcStage;
		VkPipelineStageFlags dstStage;

		if (old == VK_IMAGE_LAYOUT_UNDEFINED && _new == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) {
			imgMemBarrier.srcAccessMask = 0;
			imgMemBarrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

			srcStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
			dstStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
		}
		else if (old == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && _new == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) {
			imgMemBarrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
			imgMemBarrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

			srcStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
			dstStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
		}
		else {
			VKR_CHECK(VK_ERROR_UNKNOWN, "image transition not supported");
		}

		vkCmdPipelineBarrier(buff, srcStage, dstStage, 0, 0, NULL, 0, NULL, 1, &imgMemBarrier);

		endTinyCommands(buff);
	}

	void VulkanRenderInterface::changeColoredImgLayout(VkImage img, VkImageLayout old, VkImageLayout _new, u32 format) {
		changeImgLayoutWithMipsAndAspect(img, old, _new, format, VK_IMAGE_ASPECT_COLOR_BIT, 1, 0);
	}

	void VulkanRenderInterface::genMipMaps(VkImage image, VkFormat format, u32 width, u32 height, u32 num) {
		if (containsFormatWithFeatures({ format }, VK_IMAGE_TILING_OPTIMAL, VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT) == VK_FORMAT_UNDEFINED)
		VKR_CHECK(VK_ERROR_UNKNOWN, "format does not support generating mip maps!");

		VkCommandBuffer buff = beginTinyCommands();

		VkImageMemoryBarrier memBarrier;
		memBarrier.sType = VKST(IMAGE_MEMORY_BARRIER);
		memBarrier.pNext = NULL;
		memBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		memBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		memBarrier.image = image;
		memBarrier.subresourceRange = { VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1 };

		i32 mipWidth = width, mipHeight = height;

		for (u32 i = 1; i < num; i++) {
			memBarrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
			memBarrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
			memBarrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
			memBarrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
			memBarrier.subresourceRange.baseMipLevel = i - 1;
			vkCmdPipelineBarrier(buff, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0, 0, NULL, 0, NULL, 1, &memBarrier);

			VkImageBlit blit;
			blit.srcSubresource = { VK_IMAGE_ASPECT_COLOR_BIT, i - 1, 0, 1 };
			blit.srcOffsets[0] = { 0, 0, 0 };
			blit.srcOffsets[1] = { mipWidth, mipHeight, 1 };

			if (mipWidth > 1) mipWidth /= 2;
			if (mipHeight > 1) mipHeight /= 2;

			blit.dstSubresource = { VK_IMAGE_ASPECT_COLOR_BIT, i, 0, 1 };
			blit.dstOffsets[0] = { 0, 0, 0 };
			blit.dstOffsets[1] = { mipWidth, mipHeight, 1 };

			vkCmdBlitImage(buff, image, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &blit, VK_FILTER_LINEAR);

			memBarrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
			memBarrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
			memBarrier.srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
			memBarrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
			vkCmdPipelineBarrier(buff, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0, 0, NULL, 0, NULL, 1, &memBarrier);
		};

		memBarrier.subresourceRange.baseMipLevel = num - 1;
		memBarrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
		memBarrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		memBarrier.srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
		memBarrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
		vkCmdPipelineBarrier(buff, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0, 0, NULL, 0, NULL, 1, &memBarrier);

		endTinyCommands(buff);
	}

	VksImage VulkanRenderInterface::createSampledImageWithMips(u32 width, u32 height, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkSampleCountFlags samples, u32 mipLevels) {
		VkImageCreateInfo imageCI{};
		imageCI.sType = VKCI(IMAGE);
		imageCI.pNext = NULL;
		imageCI.flags = 0;
		imageCI.extent = { width, height, 1 };
		imageCI.mipLevels = mipLevels;
		imageCI.arrayLayers = 1;
		imageCI.format = format;
		imageCI.tiling = tiling;
		imageCI.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		imageCI.usage = usage;
		imageCI.samples = static_cast<VkSampleCountFlagBits>(samples);
		imageCI.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
		imageCI.imageType = VK_IMAGE_TYPE_2D;

		VksImage img;
		VKR_CHECK(vkCreateImage(device, &imageCI, NULL, &img.handle), "failed to create texture!");

		VkMemoryRequirements memReqs{};
		vkGetImageMemoryRequirements(device, img.handle, &memReqs);

		VkMemoryAllocateInfo texMemAllocInfo{};
		texMemAllocInfo.sType = VKST(MEMORY_ALLOCATE_INFO);
		texMemAllocInfo.pNext = NULL;
		texMemAllocInfo.allocationSize = memReqs.size;
		texMemAllocInfo.memoryTypeIndex = findMemType(memReqs, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

		VKR_CHECK(vkAllocateMemory(device, &texMemAllocInfo, NULL, &img.mem), "failed to allocate texture memory!");

		vkBindImageMemory(device, img.handle, img.mem, 0);
		return img;
	}

	VksImage VulkanRenderInterface::createImage(u32 width, u32 height, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage) {
		return createSampledImageWithMips(width, height, format, tiling, usage, VK_SAMPLE_COUNT_1_BIT, 1);
	}

	VksImage VulkanRenderInterface::createSampledImage(u32 width, u32 height, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkSampleCountFlags samples) {
		return createSampledImageWithMips(width, height, format, tiling, usage, samples, 1);
	}

	VksImage VulkanRenderInterface::createImageWithMips(u32 width, u32 height, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, u32 mips) {
		return createSampledImageWithMips(width, height, format, tiling, usage, VK_SAMPLE_COUNT_1_BIT, mips);
	}

	void VulkanRenderInterface::fillImage(VkImage img, u32 width, u32 height, Color* pixels, u32 format) {
		VksBuffer stage = createStagingBuffer(pixels, sizeof(Color) * width * height);
		changeColoredImgLayout(img, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, format);
		copyBufferToImage(stage.handle, img, width, height);
		freeBuffer(stage);
		changeColoredImgLayout(img, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, format);
	}

	void VulkanRenderInterface::fillImageWithMips(VkImage img, u32 width, u32 height, Color* pixels, VkFormat format, u32 mips) {
		VksBuffer stage = createStagingBuffer( pixels, sizeof(Color) * width * height);
		changeImgLayoutWithMipsAndAspect(img, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, format, VK_IMAGE_ASPECT_COLOR_BIT, mips, 0);
		copyBufferToImage(stage.handle, img, width, height);
		freeBuffer(stage);
		genMipMaps(img, format, width, height, mips);
	}

	VkImageView VulkanRenderInterface::createImageViewWithMipAndType(VkImage img, VkFormat format, VkImageViewType viewType, u32 aspect, u32 baseMip, u32 mips) {
		VkImageViewCreateInfo imgViewCI{};
		imgViewCI.sType = VKCI(IMAGE_VIEW);
		imgViewCI.pNext = NULL;
		imgViewCI.flags = 0;
		imgViewCI.image = img;
		imgViewCI.viewType = viewType;
		imgViewCI.format = static_cast<VkFormat>(format);
		imgViewCI.components = { VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY , VK_COMPONENT_SWIZZLE_IDENTITY , VK_COMPONENT_SWIZZLE_IDENTITY };
		imgViewCI.subresourceRange = { aspect, baseMip, mips, 0, 1 };
		
		VkImageView imgView;
		VKR_CHECK(vkCreateImageView(device, &imgViewCI, NULL, &imgView), "failed to create image view!");
		return imgView;
	}

	VkImageView VulkanRenderInterface::createImageView2DWithMip(VkImage img, VkFormat format, u32 aspect, u32 baseMip, u32 mips) {
		return createImageViewWithMipAndType(img, format, VK_IMAGE_VIEW_TYPE_2D, aspect, baseMip, mips);
	}

	VkImageView VulkanRenderInterface::createImageView2D(VkImage img, VkFormat format, u32 aspect) {
		return createImageViewWithMipAndType(img, format, VK_IMAGE_VIEW_TYPE_2D, aspect, 0, 1);
	}

	VkSampler VulkanRenderInterface::createSamplerWithMips(u32 mips, VkFilter filter, VkSamplerAddressMode uvmode) {
		VkSamplerCreateInfo samplerCI{};
		samplerCI.sType = VKCI(SAMPLER);
		samplerCI.pNext = NULL;
		samplerCI.flags = 0;
		samplerCI.magFilter = filter;
		samplerCI.minFilter = filter;
		samplerCI.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
		samplerCI.addressModeU = uvmode;
		samplerCI.addressModeV = uvmode;
		samplerCI.addressModeW = uvmode;
		samplerCI.mipLodBias = 0.0F;
		
		VkPhysicalDeviceProperties props{};
		vkGetPhysicalDeviceProperties(gpu, &props);
		samplerCI.anisotropyEnable = VK_TRUE;
		samplerCI.maxAnisotropy = props.limits.maxSamplerAnisotropy;
		samplerCI.compareEnable = VK_FALSE;
		samplerCI.compareOp = VK_COMPARE_OP_ALWAYS;
		samplerCI.minLod = 0.0f;
		samplerCI.maxLod = static_cast<f32>(mips);
		samplerCI.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
		samplerCI.unnormalizedCoordinates = VK_FALSE;

		VkSampler sampler;
		VKR_CHECK(vkCreateSampler(device, &samplerCI, NULL, &sampler), "failed to create sampler!");
		return sampler;
	}

	VkSampler VulkanRenderInterface::createSamplerLinearRepeatWithMips(u32 mips) {
		return createSamplerWithMips(mips, VK_FILTER_LINEAR, VK_SAMPLER_ADDRESS_MODE_REPEAT);
	}

	VkSampler VulkanRenderInterface::createSamplerLinearRepeat() {
		return createSamplerWithMips(1, VK_FILTER_LINEAR, VK_SAMPLER_ADDRESS_MODE_REPEAT);
	}

	VkShaderModule VulkanRenderInterface::createShaderModule(Buffer* code) {
		VkShaderModuleCreateInfo shaderModCI;
		shaderModCI.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
		shaderModCI.pNext = NULL;
		shaderModCI.flags = 0;
		shaderModCI.codeSize = code->size;
		shaderModCI.pCode = as<u32*>(code->data);

		VkShaderModule shader;
		VKR_CHECK(vkCreateShaderModule(device, &shaderModCI, NULL, &shader), "failed to create vulkan shader module!");
		return shader;
	}

	bool VulkanRenderInterface::initialize(Window& window) {
		const char* instanceExtensions[] = {"VK_KHR_surface", VULKAN_WINDOW_SURFACE_NAME};
		const char* instanceLayers[] = {"VK_LAYER_KHRONOS_validation"};
		
		VkApplicationInfo appInfo;
		appInfo.sType = VKST(APPLICATION_INFO);
		appInfo.pNext = NULL;
		appInfo.pApplicationName = "Vulkan Engine";
		appInfo.applicationVersion = VK_MAKE_VERSION(0, 1, 0);
		appInfo.pEngineName = "Vulkan Engine";
		appInfo.engineVersion = VK_MAKE_VERSION(0, 1, 0);
		appInfo.apiVersion = VK_API_VERSION_1_2;

		VkInstanceCreateInfo instanceInfo;
		instanceInfo.sType = VKCI(INSTANCE);
		instanceInfo.pNext = NULL;
		instanceInfo.flags = 0;
		instanceInfo.pApplicationInfo = &appInfo;
		instanceInfo.enabledLayerCount = 1;
		instanceInfo.ppEnabledLayerNames = instanceLayers;
		instanceInfo.enabledExtensionCount = 2;
		instanceInfo.ppEnabledExtensionNames = instanceExtensions;
		VKR_CHECK(vkCreateInstance(&instanceInfo, NULL, &instance), "instance creation failed!");

		getPhysicalDevice(true);

		float queuePriority = 1.0F;
		VkDeviceQueueCreateInfo queueInfos[1];
		queueInfos[0].sType = VKCI(DEVICE_QUEUE);
		queueInfos[0].pNext = NULL;
		queueInfos[0].flags = 0;
		queueInfos[0].queueCount = 1;
		queueInfos[0].queueFamilyIndex = getQueueFamily(VK_QUEUE_GRAPHICS_BIT);
		queueInfos[0].pQueuePriorities = &queuePriority;

		const char* deviceExtensions[] = {VK_KHR_SWAPCHAIN_EXTENSION_NAME};
		VkDeviceCreateInfo deviceInfo;
		deviceInfo.sType = VKCI(DEVICE);
		deviceInfo.pNext = NULL;
		deviceInfo.flags = 0;
		deviceInfo.queueCreateInfoCount = 1;
		deviceInfo.pQueueCreateInfos = queueInfos;
		deviceInfo.enabledLayerCount = 0;
		deviceInfo.ppEnabledLayerNames = NULL;
		deviceInfo.enabledExtensionCount = 1;
		deviceInfo.ppEnabledExtensionNames = deviceExtensions;

		VkPhysicalDeviceFeatures gpuFeatures{};
		vkGetPhysicalDeviceFeatures(gpu, &gpuFeatures);
		enabledFeatures.samplerAnisotropy = gpuFeatures.samplerAnisotropy;

		deviceInfo.pEnabledFeatures = &enabledFeatures;
		VKR_CHECK(vkCreateDevice(gpu, &deviceInfo, NULL, &device), "device creation failed!");

		createCmdPool(getQueueFamily(VK_QUEUE_GRAPHICS_BIT));
		msaa = VK_SAMPLE_COUNT_8_BIT;

#if SGE_PLATFORM_WINDOWS

		VkWin32SurfaceCreateInfoKHR winSurfCI{};
		winSurfCI.sType = VKCIK(WIN32_SURFACE);
		winSurfCI.pNext = NULL;
		winSurfCI.flags = 0;
		winSurfCI.hinstance = as<HINSTANCE>(window.appInstance);
		winSurfCI.hwnd = as<HWND>(window.handle);

		VKR_CHECK(vkCreateWin32SurfaceKHR(instance, &winSurfCI, NULL, &surface), "failed to create surface");

#elif SGE_PLATFORM_LINUX

		VkXcbSurfaceCreateInfoKHR xcbSurfCI{};
		xcbSurfCI.sType = VKCIK(XCB_SURFACE);
		xcbSurfCI.pNext = NULL;
		xcbSurfCI.flags = 0;
		xcbSurfCI.connection = (xcb_connection_t*)window.appInstance;
		xcbSurfCI.window = window.handle;

		VKR_CHECK(vkCreateXcbSurfaceKHR(instance, &xcbSurfCI, NULL, &surface), "failed to create surface");

#endif // SGE_PLATFORM
		
		getPresentMode();
		getSurfaceFormat();
		depthFormat = VK_FORMAT_D32_SFLOAT;

		VkSurfaceCapabilitiesKHR caps;
		vkGetPhysicalDeviceSurfaceCapabilitiesKHR(gpu, surface, &caps);
		width = caps.currentExtent.width;
		height = caps.currentExtent.height;
		minImgCount = caps.minImageCount;
		maxImgCount = caps.maxImageCount;
		transform = caps.currentTransform;

		VkAttachmentDescription colorAttachment{};
		colorAttachment.flags = 0;
		colorAttachment.format = format.format;
		colorAttachment.samples = static_cast<VkSampleCountFlagBits>(msaa);
		colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
		colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

		VkAttachmentReference colorAttachmentRef{};
		colorAttachmentRef.attachment = 0;
		colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

		VkAttachmentDescription colorAttachmentResolve{};
		colorAttachmentResolve.flags = 0;
		colorAttachmentResolve.format = format.format;
		colorAttachmentResolve.samples = VK_SAMPLE_COUNT_1_BIT;
		colorAttachmentResolve.loadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		colorAttachmentResolve.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
		colorAttachmentResolve.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		colorAttachmentResolve.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		colorAttachmentResolve.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		colorAttachmentResolve.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

		VkAttachmentReference colorAttachmentResolveRef{};
		colorAttachmentResolveRef.attachment = 2;
		colorAttachmentResolveRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

		VkAttachmentDescription depthAttachment{};
		depthAttachment.flags = 0;
		depthAttachment.format = static_cast<VkFormat>(depthFormat);
		depthAttachment.samples = static_cast<VkSampleCountFlagBits>(msaa);
		depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		depthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		depthAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		depthAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		depthAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		depthAttachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

		VkAttachmentReference depthAttachmentRef{};
		depthAttachmentRef.attachment = 1;
		depthAttachmentRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

		VkSubpassDescription subpass{};
		subpass.flags = 0;
		subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
		subpass.colorAttachmentCount = 1;
		subpass.pColorAttachments = &colorAttachmentRef;
		subpass.pDepthStencilAttachment = &depthAttachmentRef;

		VkSubpassDependency subPassDependency{};
		subPassDependency.srcSubpass = VK_SUBPASS_EXTERNAL;
		subPassDependency.dstSubpass = 0;
		subPassDependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
		subPassDependency.srcAccessMask = 0;
		subPassDependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
		subPassDependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

		sge::Array<VkAttachmentDescription> attachments;
		
		if (msaa != VK_SAMPLE_COUNT_1_BIT) {
			colorAttachment.finalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
			subpass.pResolveAttachments = &colorAttachmentResolveRef;
			attachments.append({colorAttachment, depthAttachment, colorAttachmentResolve});
		}
		else {
			attachments.append({colorAttachment, depthAttachment});
		}

		VkRenderPassCreateInfo rpInfo{};
		rpInfo.sType = VKCI(RENDER_PASS);
		rpInfo.pNext = NULL;
		rpInfo.flags = 0;
		rpInfo.attachmentCount = attachments.size;
		rpInfo.pAttachments = attachments.data;
		rpInfo.subpassCount = 1;
		rpInfo.pSubpasses = &subpass;
		rpInfo.dependencyCount = 1;
		rpInfo.pDependencies = &subPassDependency;
		VKR_CHECK(vkCreateRenderPass(device, &rpInfo, NULL, &renderPass), "renderpass creation failed!");

		VkBool32 surfaceSupport;
		vkGetPhysicalDeviceSurfaceSupportKHR(gpu, cmdPool.family, surface, &surfaceSupport);

		VkSwapchainCreateInfoKHR swapchainInfo;
		swapchainInfo.sType = VKCIK(SWAPCHAIN);
		swapchainInfo.pNext = nullptr;
		swapchainInfo.flags = 0;
		swapchainInfo.surface = surface;
		swapchainInfo.minImageCount = minImgCount + 1 > maxImgCount && maxImgCount > 0 ? maxImgCount : minImgCount + 1;
		swapchainInfo.imageFormat = format.format;
		swapchainInfo.imageColorSpace = format.colorSpace;
		swapchainInfo.imageExtent = {width, height};
		swapchainInfo.imageArrayLayers = 1;
		swapchainInfo.imageUsage = VKIU(COLOR_ATTACHMENT);
		swapchainInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
		swapchainInfo.queueFamilyIndexCount = 1;
		swapchainInfo.pQueueFamilyIndices = &cmdPool.family;
		swapchainInfo.preTransform = static_cast<VkSurfaceTransformFlagBitsKHR>(transform);
		swapchainInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
		swapchainInfo.presentMode = presentMode;
		swapchainInfo.clipped = VK_TRUE;
		swapchainInfo.oldSwapchain = VK_NULL_HANDLE;
		VKR_CHECK(vkCreateSwapchainKHR(device, &swapchainInfo, NULL, &swapchain), "swapchain creation failed!");

		vkGetSwapchainImagesKHR(device, swapchain, &imgCount, NULL);
		imgs = new VkImage[imgCount];
		vkGetSwapchainImagesKHR(device, swapchain, &imgCount, imgs);

		views = new VkImageView[imgCount];
		for (u32 i = 0; i < imgCount; i++) {
			views[i] = createImageView2D(imgs[i], format.format, VK_IMAGE_ASPECT_COLOR_BIT);
		}

		colImg = createSampledImage(width, height, format.format, VK_IMAGE_TILING_OPTIMAL, VKIU(COLOR_ATTACHMENT) | VKIU(TRANSIENT_ATTACHMENT), msaa);
		colImg.view = createImageView2D(colImg.handle, format.format, VK_IMAGE_ASPECT_COLOR_BIT);
		depthImg = createSampledImage(width, height, depthFormat, VK_IMAGE_TILING_OPTIMAL, VKIU(DEPTH_STENCIL_ATTACHMENT), msaa);
		depthImg.view = createImageView2D(depthImg.handle, depthFormat, VK_IMAGE_ASPECT_DEPTH_BIT);

		VkFramebufferCreateInfo fbInfo{};
		fbInfo.sType = VKCI(FRAMEBUFFER);
		fbInfo.pNext = NULL;
		fbInfo.flags = 0;
		fbInfo.renderPass = renderPass;
		fbInfo.width = width;
		fbInfo.height = height;
		fbInfo.layers = 1;

		framebuffers = new VkFramebuffer[imgCount];
		VkImageView* fbAttachments;
		for (u32 i = 0; i < imgCount; i++) {
			if (msaa == VK_SAMPLE_COUNT_1_BIT) {
				fbAttachments = new VkImageView[2] {views[i], depthImg.view};
				fbInfo.attachmentCount = 2;
			}
			else {

				fbAttachments = new VkImageView[3] {colImg.view, depthImg.view, views[i]};
				fbInfo.attachmentCount = 3;
			}
			fbInfo.pAttachments = fbAttachments;
			VKR_CHECK(vkCreateFramebuffer(device, &fbInfo, NULL, &framebuffers[i]), "framebuffer creation failed!");
		}

		parallelFrameCount = 2 > imgCount ? imgCount : 2;
		gotImgSemaphores = new VkSemaphore[parallelFrameCount];
		doneSemaphores = new VkSemaphore[parallelFrameCount];
		renderFences = new VkFence[parallelFrameCount];
		CmdBuffers = allocCmdBuffers(parallelFrameCount, VK_COMMAND_BUFFER_LEVEL_PRIMARY);

		VkFenceCreateInfo fenceInfo;
		fenceInfo.sType = VKCI(FENCE);
		fenceInfo.pNext = NULL;
		fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;
		VkSemaphoreCreateInfo semaphoreInfo;
		semaphoreInfo.sType = VKCI(SEMAPHORE);
		semaphoreInfo.pNext = NULL;
		semaphoreInfo.flags = 0;

		for (u32 i = 0; i < parallelFrameCount; i++) {
			vkCreateSemaphore(device, &semaphoreInfo, NULL, &gotImgSemaphores[i]);
			vkCreateSemaphore(device, &semaphoreInfo, NULL, &doneSemaphores[i]);
			vkCreateFence(device, &fenceInfo, NULL, &renderFences[i]);
		}

		frameMap = new u32[imgCount];
		for (u32 i = 0; i < imgCount; i++){
			frameMap[i] = 32;
		}

		prepareStuff();
		prepareFrameDependantStuff();
		return true;
	}

	void VulkanRenderInterface::onWindowResized() {
		windowResized = true;
	}

	void VulkanRenderInterface::destroy() {
		vkDeviceWaitIdle(device);

		destroyFrameDependantStuff();
		destroyStuff();

		vkFreeCommandBuffers(device, cmdPool.handle, parallelFrameCount, CmdBuffers);
		for (u32 i = 0; i < parallelFrameCount; i++) {
			vkDestroySemaphore(device, gotImgSemaphores[i], NULL);
			vkDestroySemaphore(device, doneSemaphores[i], NULL);
			vkDestroyFence(device, renderFences[i], NULL);
		}
		delete[] gotImgSemaphores;
		delete[] doneSemaphores;
		delete[] renderFences;
		delete[] frameMap;
		vkDestroyImageView(device, colImg.view, NULL);
		vkDestroyImage(device, colImg.handle, NULL);
		vkFreeMemory(device, colImg.mem, NULL);
		
		vkDestroyImageView(device, depthImg.view, NULL);
		vkDestroyImage(device, depthImg.handle, NULL);
		vkFreeMemory(device, depthImg.mem, NULL);
		for (u32 i = 0; i < imgCount; i++) {
			vkDestroyFramebuffer(device, framebuffers[i], NULL);
			vkDestroyImageView(device, views[i], NULL);
		}
		delete[] framebuffers;
		delete[] views;
		delete[] imgs;
		vkDestroySwapchainKHR(device, swapchain, NULL);
		vkDestroyRenderPass(device, renderPass, NULL);
		vkDestroySurfaceKHR(instance, surface, NULL);
		vkDestroyCommandPool(device, cmdPool.handle, NULL);
		vkDestroyDevice(device, NULL);
		vkDestroyInstance(instance, NULL);
	}

	bool VulkanRenderInterface::startFrame() {
		syncFrame = (syncFrame + 1) % parallelFrameCount;
		VkResult error = vkAcquireNextImageKHR(device, swapchain, UINT64_MAX, gotImgSemaphores[syncFrame], VK_NULL_HANDLE, &currentImg);
		
		if (error == VK_ERROR_OUT_OF_DATE_KHR || windowResized) {
			vkDeviceWaitIdle(device);

			// Destroy old objects
			destroyFrameDependantStuff();
			vkFreeCommandBuffers(device, cmdPool.handle, parallelFrameCount, CmdBuffers);
			for (u32 i = 0; i < parallelFrameCount; i++) {
				vkDestroySemaphore(device, gotImgSemaphores[i], NULL);
				vkDestroySemaphore(device, doneSemaphores[i], NULL);
				vkDestroyFence(device, renderFences[i], NULL);
			}
			delete[] gotImgSemaphores;
			delete[] doneSemaphores;
			delete[] renderFences;
			delete[] frameMap;

			vkDestroyImageView(device, colImg.view, NULL);
			vkDestroyImage(device, colImg.handle, NULL);
			vkFreeMemory(device, colImg.mem, NULL);
			
			vkDestroyImageView(device, depthImg.view, NULL);
			vkDestroyImage(device, depthImg.handle, NULL);
			vkFreeMemory(device, depthImg.mem, NULL);

			for (u32 i = 0; i < imgCount; i++) {
				vkDestroyFramebuffer(device, framebuffers[i], NULL);
				vkDestroyImageView(device, views[i], NULL);
			}
			delete[] framebuffers;
			delete[] views;
			delete[] imgs;
			vkDestroySwapchainKHR(device, swapchain, NULL);

			// Create new objects
			VkSurfaceCapabilitiesKHR caps;
			vkGetPhysicalDeviceSurfaceCapabilitiesKHR(gpu, surface, &caps);
			width = caps.currentExtent.width;
			height = caps.currentExtent.height;
			minImgCount = caps.minImageCount;
			maxImgCount = caps.maxImageCount;
			transform = caps.currentTransform;

			VkSwapchainCreateInfoKHR swapchainInfo;
			swapchainInfo.sType = VKCIK(SWAPCHAIN);
			swapchainInfo.pNext = nullptr;
			swapchainInfo.flags = 0;
			swapchainInfo.surface = surface;
			swapchainInfo.minImageCount = minImgCount + 1 > maxImgCount && maxImgCount > 0 ? maxImgCount : minImgCount + 1;
			swapchainInfo.imageFormat = format.format;
			swapchainInfo.imageColorSpace = format.colorSpace;
			swapchainInfo.imageExtent = {width, height};
			swapchainInfo.imageArrayLayers = 1;
			swapchainInfo.imageUsage = VKIU(COLOR_ATTACHMENT);
			swapchainInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
			swapchainInfo.queueFamilyIndexCount = 1;
			swapchainInfo.pQueueFamilyIndices = &cmdPool.family;
			swapchainInfo.preTransform = static_cast<VkSurfaceTransformFlagBitsKHR>(transform);
			swapchainInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
			swapchainInfo.presentMode = presentMode;
			swapchainInfo.clipped = VK_TRUE;
			swapchainInfo.oldSwapchain = VK_NULL_HANDLE;
			VKR_CHECK(vkCreateSwapchainKHR(device, &swapchainInfo, NULL, &swapchain), "swapchain creation failed!");

			vkGetSwapchainImagesKHR(device, swapchain, &imgCount, NULL);
			imgs = new VkImage[imgCount];
			vkGetSwapchainImagesKHR(device, swapchain, &imgCount, imgs);

			views = new VkImageView[imgCount];
			for (u32 i = 0; i < imgCount; i++) {
				views[i] = createImageView2D(imgs[i], format.format, VK_IMAGE_ASPECT_COLOR_BIT);
			}

			colImg = createSampledImage(width, height, format.format, VK_IMAGE_TILING_OPTIMAL, VKIU(COLOR_ATTACHMENT) | VKIU(TRANSIENT_ATTACHMENT), msaa);
			colImg.view = createImageView2D(colImg.handle, format.format, VK_IMAGE_ASPECT_COLOR_BIT);
			depthImg = createSampledImage(width, height, depthFormat, VK_IMAGE_TILING_OPTIMAL, VKIU(DEPTH_STENCIL_ATTACHMENT), msaa);
			depthImg.view = createImageView2D(depthImg.handle, depthFormat, VK_IMAGE_ASPECT_DEPTH_BIT);

			VkFramebufferCreateInfo fbInfo{};
			fbInfo.sType = VKCI(FRAMEBUFFER);
			fbInfo.pNext = NULL;
			fbInfo.flags = 0;
			fbInfo.renderPass = renderPass;
			fbInfo.width = width;
			fbInfo.height = height;
			fbInfo.layers = 1;

			framebuffers = new VkFramebuffer[imgCount];
			VkImageView* fbAttachments;
			for (u32 i = 0; i < imgCount; i++) {
				if (msaa == VK_SAMPLE_COUNT_1_BIT) {
					fbAttachments = new VkImageView[2] {views[i], depthImg.view};
					fbInfo.attachmentCount = 2;
				}
				else {

					fbAttachments = new VkImageView[3] {colImg.view, depthImg.view, views[i]};
					fbInfo.attachmentCount = 3;
				}
				fbInfo.pAttachments = fbAttachments;
				VKR_CHECK(vkCreateFramebuffer(device, &fbInfo, NULL, &framebuffers[i]), "framebuffer creation failed!");
			}

			parallelFrameCount = 2 > imgCount ? imgCount : 2;
			gotImgSemaphores = new VkSemaphore[parallelFrameCount];
			doneSemaphores = new VkSemaphore[parallelFrameCount];
			renderFences = new VkFence[parallelFrameCount];
			CmdBuffers = allocCmdBuffers(parallelFrameCount, VK_COMMAND_BUFFER_LEVEL_PRIMARY);

			VkFenceCreateInfo fenceInfo;
			fenceInfo.sType = VKCI(FENCE);
			fenceInfo.pNext = NULL;
			fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;
			VkSemaphoreCreateInfo semaphoreInfo;
			semaphoreInfo.sType = VKCI(SEMAPHORE);
			semaphoreInfo.pNext = NULL;
			semaphoreInfo.flags = 0;

			for (u32 i = 0; i < parallelFrameCount; i++) {
				vkCreateSemaphore(device, &semaphoreInfo, NULL, &gotImgSemaphores[i]);
				vkCreateSemaphore(device, &semaphoreInfo, NULL, &doneSemaphores[i]);
				vkCreateFence(device, &fenceInfo, NULL, &renderFences[i]);
			}

			frameMap = new u32[imgCount];
			for (u32 i = 0; i < imgCount; i++){
				frameMap[i] = 32;
			}

			prepareFrameDependantStuff();
			windowResized = false;
			return false;
		}

		if (frameMap[currentImg] < parallelFrameCount) {
			vkWaitForFences(device, 1, &renderFences[frameMap[currentImg]], VK_TRUE, UINT64_MAX);
		}
		else {
			vkWaitForFences(device, 1, &renderFences[syncFrame], VK_TRUE, UINT64_MAX);
			frameMap[currentImg] = syncFrame;
		}
		currentFrame = frameMap[currentImg];
		drawCmdBuffer = CmdBuffers[currentFrame];

		updateFrameDependantStuff();

		vkResetFences(device, 1, &renderFences[currentFrame]);

		VkCommandBufferBeginInfo beginInfo;
		beginInfo.sType = VKST(COMMAND_BUFFER_BEGIN_INFO);
		beginInfo.pNext = NULL;
		beginInfo.flags = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;
		beginInfo.pInheritanceInfo = NULL;
		vkBeginCommandBuffer(drawCmdBuffer, &beginInfo);

		VkClearValue clearValues[2];
		clearValues[0].color = {0.0F, 0.0F, 0.0F, 1.0F};
		clearValues[1].depthStencil = {0.0F, 0};

		VkRenderPassBeginInfo rpBeginInfo;
		rpBeginInfo.sType = VKST(RENDER_PASS_BEGIN_INFO);
		rpBeginInfo.pNext = NULL;
		rpBeginInfo.renderPass = renderPass;
		rpBeginInfo.framebuffer = framebuffers[currentImg];
		rpBeginInfo.renderArea = { {0, 0}, {width, height} };
		rpBeginInfo.clearValueCount = 2;
		rpBeginInfo.pClearValues = clearValues;
		vkCmdBeginRenderPass(drawCmdBuffer, &rpBeginInfo, VK_SUBPASS_CONTENTS_INLINE);

		VkViewport viewport{0, 0, (float)width, (float)height, 0.0F, 1.0F};
		vkCmdSetViewport(drawCmdBuffer, 0, 1, &viewport);

		VkRect2D scissor{{0, 0}, {width, height}};
		vkCmdSetScissor(drawCmdBuffer, 0, 1, &scissor);
		return true;
	}

	void VulkanRenderInterface::endFrame() {
		vkCmdEndRenderPass(drawCmdBuffer);
		vkEndCommandBuffer(drawCmdBuffer);

		VkSubmitInfo submitInfo{};
		submitInfo.sType = VKST(SUBMIT_INFO);
		submitInfo.pNext = NULL;
		submitInfo.waitSemaphoreCount = 1;
		submitInfo.pWaitSemaphores = &gotImgSemaphores[syncFrame];
		VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
		submitInfo.pWaitDstStageMask = waitStages;
		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &CmdBuffers[currentFrame];
		submitInfo.signalSemaphoreCount = 1;
		submitInfo.pSignalSemaphores = &doneSemaphores[syncFrame];

		vkQueueSubmit(cmdPool.queue, 1, &submitInfo, renderFences[currentFrame]);

		VkPresentInfoKHR presentInfo{};
		presentInfo.sType = VKST(PRESENT_INFO_KHR);
		presentInfo.pNext = NULL;
		presentInfo.waitSemaphoreCount = 1;
		presentInfo.pWaitSemaphores = &doneSemaphores[syncFrame];
		presentInfo.swapchainCount = 1;
		presentInfo.pSwapchains = &swapchain;
		presentInfo.pImageIndices = &currentImg;
		presentInfo.pResults = NULL;
		vkQueuePresentKHR(cmdPool.queue, &presentInfo);
	}
}