#pragma once

#include "lve_window.hpp"
#include "lve_pipeline.hpp"
#include "lve_device.hpp"
#include "lve_swap_chain.hpp"
#include "model.hpp"
#include "lve_types.hpp"

// std
#include <memory>
#include <vector>

namespace lve {
	class FirstApp {
	public:
		static constexpr int WIDTH = 800;
		static constexpr int HEIGHT = 600;

		const std::string MODEL_PATH = "resources/models/viking_room.obj";
		const std::string TEXTURE_PATH = "resources/textures/viking_room.png";

		FirstApp();
		~FirstApp();
		
		FirstApp(const FirstApp&) = delete;
		FirstApp& operator=(const FirstApp&) = delete;
		void run();

	private:
		void createCommandBuffers();
		void createTextureImage();
		void drawFrame();
		void loadModels();
		void updateUniformBuffer(uint32_t currentImage);

		LveWindow lveWindow{ WIDTH, HEIGHT, "Hello Vulkan!" };
		LveDevice lveDevice{ lveWindow };
		LveSwapChain lveSwapChain{ lveDevice, lveWindow.getExtent() };
		std::vector<VkCommandBuffer> commandBuffers;
		std::unique_ptr<Model> model;

		ApplicationPipelines applicationPipelines;
		AllocatedImage textureImage;
		VkSampler textureSampler;
	};
}