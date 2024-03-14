#pragma once

#include "lve_window.hpp"
#include "lve_device.hpp"
#include "lve_swap_chain.hpp"
#include "gui.hpp"
#include "model.hpp"
#include "lve_types.hpp"
#include "scene_manager.hpp"

// std
#include <memory>
#include <vector>

namespace lve {
	class FirstApp {
	public:
		static constexpr int WIDTH = 1600;
		static constexpr int HEIGHT = 900;

		FirstApp();
		~FirstApp();
		
		FirstApp(const FirstApp&) = delete;
		FirstApp& operator=(const FirstApp&) = delete;
		void run();

	private:
		void createCommandBuffers();
		void drawFrame();

		LveWindow lveWindow{ WIDTH, HEIGHT, "Vulkan Renderer" };
		LveDevice lveDevice{ lveWindow };
		LveSwapChain lveSwapChain{ lveDevice, lveWindow.getExtent() };
		LveGui lveGui { lveDevice, lveSwapChain, lveWindow };
		std::vector<VkCommandBuffer> commandBuffers;

		ApplicationPipelines applicationPipelines;
		std::unique_ptr<SceneManager> sceneManager;
	};
}