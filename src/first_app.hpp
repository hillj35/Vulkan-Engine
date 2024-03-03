#pragma once

#include "lve_window.hpp"
#include "lve_pipeline.hpp"
#include "lve_device.hpp"
#include "lve_swap_chain.hpp"
#include "model.hpp"

// std
#include <memory>
#include <vector>

namespace lve {
	class FirstApp {
	public:
		static constexpr int WIDTH = 800;
		static constexpr int HEIGHT = 600;

		FirstApp();
		~FirstApp();
		
		FirstApp(const FirstApp&) = delete;
		FirstApp& operator=(const FirstApp&) = delete;
		void run();

	private:
		void createCommandBuffers();
		void createDescriptorSetLayout();
		void createImage(uint32_t width, uint32_t height, VkFormat format, VkImageTiling tiling, 
			VkImageUsageFlags usage, VkMemoryPropertyFlags properties, VkImage& image, VkDeviceMemory& imageMemory);
		void createPipelineLayout();
		void createPipeline();
		void createTextureImage();
		void createTextureImageView();
		void createTextureSampler();
		void drawFrame();
		void loadModels();
		void transitionImageLayout(VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout);
		void updateUniformBuffer(uint32_t currentImage);

		LveWindow lveWindow{ WIDTH, HEIGHT, "Hello Vulkan!" };
		LveDevice lveDevice{ lveWindow };
		LveSwapChain lveSwapChain{ lveDevice, lveWindow.getExtent() };
		std::unique_ptr<LvePipeline> lvePipeline;
		VkPipelineLayout pipelineLayout;
		VkDescriptorSetLayout descriptorSetLayout;
		VkImage textureImage;
		VkImageView textureImageView;
		VkSampler textureSampler;
		VkDeviceMemory textureImageMemory;
		std::vector<VkCommandBuffer> commandBuffers;
		std::unique_ptr<Model> model;
	};
}