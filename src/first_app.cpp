#include "first_app.hpp"

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "initializers/pipelines.hpp"
#include "initializers/images.hpp"
#include "initializers/initializers.hpp"
#include "utility/images.hpp"

// std
#include <chrono>
#include <stdexcept>
#include <array>
#include <iostream>

namespace lve {
	FirstApp::FirstApp() {
		init::createPipelines(lveDevice.device(), &lveSwapChain, &applicationPipelines);
		init::createComputePipelines(lveDevice.device(), &lveSwapChain, &applicationPipelines);
		sceneManager = std::make_unique<SceneManager>(lveDevice, applicationPipelines);
		createCommandBuffers();
	}

	FirstApp::~FirstApp() {
		destroyApplicationPipelines(lveDevice.device(), applicationPipelines);
	}

	void FirstApp::run() {
		while (!lveWindow.shouldClose()) {
			glfwPollEvents();

			ImGui_ImplVulkan_NewFrame();
			ImGui_ImplGlfw_NewFrame();
			ImGui::NewFrame();
			sceneManager->showSceneSelectGui();
			sceneManager->getCurrentScene()->showSceneGui();

			ImGui::Render();

			drawFrame();

			if (sceneManager->shouldChangeScene()) {
				sceneManager->changeScene();
			}
		}

		vkDeviceWaitIdle(lveDevice.device());
	}

	void FirstApp::createCommandBuffers() {
		commandBuffers.resize(lveSwapChain.imageCount());

		VkCommandBufferAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		allocInfo.commandPool = lveDevice.getCommandPool();
		allocInfo.commandBufferCount = static_cast<uint32_t>(commandBuffers.size());

		if (vkAllocateCommandBuffers(lveDevice.device(), &allocInfo, commandBuffers.data()) != VK_SUCCESS) {
			throw std::runtime_error("failed to allocated command buffers");
		}
	}

	void FirstApp::drawFrame() {
		uint32_t imageIndex;
		auto result = lveSwapChain.acquireNextImage(&imageIndex);

		if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
			throw std::runtime_error("failed to acquire swap chain image");
		}

		lveSwapChain.waitForFrameFence(&imageIndex);
		
		vkResetCommandBuffer(commandBuffers[imageIndex], 0);

		VkCommandBufferBeginInfo beginInfo = init::commandBufferBeginInfo();

		if (vkBeginCommandBuffer(commandBuffers[imageIndex], &beginInfo) != VK_SUCCESS) {
			throw std::runtime_error("failed to begin recording command buffer");
		}

		sceneManager->getCurrentScene()->draw(commandBuffers[imageIndex], lveSwapChain, imageIndex, lveSwapChain.getCurrentFrame());

		lveGui.draw(commandBuffers[imageIndex], lveSwapChain.getImageView(imageIndex));

		util::transitionImageLayout(commandBuffers[imageIndex], lveSwapChain.getImage(imageIndex), lveSwapChain.getSwapChainImageFormat(), VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR);

		if (vkEndCommandBuffer(commandBuffers[imageIndex]) != VK_SUCCESS) {
			throw std::runtime_error("failed to record command buffer");
		}
	
		sceneManager->getCurrentScene()->updateUniformBuffer(lveSwapChain.getCurrentFrame(), lveSwapChain.width(), lveSwapChain.height());

		result = lveSwapChain.submitCommandBuffers(&commandBuffers[imageIndex], &imageIndex);
		if (result != VK_SUCCESS) {
			throw std::runtime_error("failed to present swap chain image");
		}
	}
}