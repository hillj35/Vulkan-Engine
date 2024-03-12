#pragma once

#include "lve_window.hpp"
#include "lve_device.hpp"
#include "lve_swap_chain.hpp"
#include "gui.hpp"
#include "model.hpp"
#include "lve_types.hpp"

// std
#include <memory>
#include <vector>

namespace lve {
	class FirstApp {
	public:
		static constexpr int WIDTH = 1600;
		static constexpr int HEIGHT = 900;

		const std::string MODEL_PATH = "resources/models/viking_room.obj";
		const std::string TEXTURE_PATH = "resources/textures/viking_room.png";
		const std::string CUBE_MODEL_PATH = "resources/models/cube.obj";
		const std::string CUBE_TEXTURE_PATH = "resources/textures/white.png";

		FirstApp();
		~FirstApp();
		
		FirstApp(const FirstApp&) = delete;
		FirstApp& operator=(const FirstApp&) = delete;
		void run();

	private:
		void createCommandBuffers();
		void createTextureImages();
		void drawFrame();
		void loadModels();
		void updateUniformBuffer(uint32_t currentImage);

		LveWindow lveWindow{ WIDTH, HEIGHT, "Hello Vulkan!" };
		LveDevice lveDevice{ lveWindow };
		LveSwapChain lveSwapChain{ lveDevice, lveWindow.getExtent() };
		LveGui lveGui { lveDevice, lveSwapChain, lveWindow };
		std::vector<VkCommandBuffer> commandBuffers;
		std::unique_ptr<Model> model;
		std::unique_ptr<Model> cubeModel;

		ApplicationPipelines applicationPipelines;
		AllocatedImage textureImage;
		AllocatedImage cubeTextureImage;
		VkSampler textureSampler;
		TransparentPushConstants cubePushConstants{};
	};
}