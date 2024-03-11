#include "first_app.hpp"

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "initializers/pipelines.hpp"
#include "initializers/images.hpp"
#include "utility/images.hpp"

// std
#include <chrono>
#include <stdexcept>
#include <array>

namespace lve {
	FirstApp::FirstApp() {
		createTextureImages();
		init::createImageSampler(lveDevice.device(), lveDevice.properties.limits.maxSamplerAnisotropy, textureSampler);
		init::createPipelines(lveDevice.device(), &lveSwapChain, &applicationPipelines);
		loadModels();
		createCommandBuffers();
	}

	FirstApp::~FirstApp() {
		vkDestroySampler(lveDevice.device(), textureSampler, nullptr);
		destroyImage(lveDevice.device(), textureImage);
		destroyImage(lveDevice.device(), cubeTextureImage);
		destroyApplicationPipelines(lveDevice.device(), applicationPipelines);
	}

	void FirstApp::run() {
		while (!lveWindow.shouldClose()) {
			glfwPollEvents();
			drawFrame();
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

		for (int i = 0; i < commandBuffers.size(); i++) {
			VkCommandBufferBeginInfo beginInfo{};
			beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

			if (vkBeginCommandBuffer(commandBuffers[i], &beginInfo) != VK_SUCCESS) {
				throw std::runtime_error("failed to begin recording command buffer");
			}

			VkRenderingAttachmentInfo colorAttachment { .sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO};
			colorAttachment.imageView = lveSwapChain.getImageView(i);
			colorAttachment.imageLayout = VK_IMAGE_LAYOUT_GENERAL;
			colorAttachment.clearValue = { 0.1f, 0.1f, 0.1f, 1.0f };
			colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
			colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;

			VkRenderingAttachmentInfo depthAttachment { .sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO};
			depthAttachment.imageView = lveSwapChain.getDepthImageView(i);
			depthAttachment.imageLayout = VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL;
			depthAttachment.clearValue.depthStencil = { 1.0f, 0 };
			depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
			depthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;

			VkRenderingInfo renderingInfo { .sType = VK_STRUCTURE_TYPE_RENDERING_INFO };
			renderingInfo.renderArea = VkRect2D { VkOffset2D {0, 0}, lveSwapChain.getSwapChainExtent()};
			renderingInfo.layerCount = 1;
			renderingInfo.colorAttachmentCount = 1;
			renderingInfo.pColorAttachments = &colorAttachment;
			renderingInfo.pDepthAttachment = &depthAttachment;
			renderingInfo.pStencilAttachment = nullptr;
			vkCmdBeginRendering(commandBuffers[i], &renderingInfo);
			vkCmdBindPipeline(commandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, applicationPipelines.opaquePipeline.pipeline);

			VkViewport viewport = {};
			viewport.x = 0;
			viewport.y = 0;
			viewport.width = lveSwapChain.getSwapChainExtent().width;
			viewport.height = lveSwapChain.getSwapChainExtent().height;
			viewport.minDepth = 0.0f;
			viewport.maxDepth = 1.0f;

			vkCmdSetViewport(commandBuffers[i], 0, 1, &viewport);

			VkRect2D scissor = {};
			scissor.offset.x = 0;
			scissor.offset.y = 0;
			scissor.extent.width = lveSwapChain.getSwapChainExtent().width;
			scissor.extent.height = lveSwapChain.getSwapChainExtent().height;

			vkCmdSetScissor(commandBuffers[i], 0, 1, &scissor);

			model->bind(commandBuffers[i], applicationPipelines.opaquePipeline.layout, lveSwapChain.getCurrentFrame());
			model->draw(commandBuffers[i]);

			vkCmdBindPipeline(commandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, applicationPipelines.transparentPipeline.pipeline);
			cubeModel->bind(commandBuffers[i], applicationPipelines.transparentPipeline.layout, lveSwapChain.getCurrentFrame());
			cubeModel->draw(commandBuffers[i]);
			vkCmdEndRendering(commandBuffers[i]);

			util::transitionImageLayout(commandBuffers[i], lveSwapChain.getImage(i), lveSwapChain.getSwapChainImageFormat(), VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR);

			if (vkEndCommandBuffer(commandBuffers[i]) != VK_SUCCESS) {
				throw std::runtime_error("failed to record command buffer");
			}
		}
	}

	void FirstApp::createTextureImages() {
		util::loadTextureImage(&lveDevice, TEXTURE_PATH, textureImage);
		util::loadTextureImage(&lveDevice, CUBE_TEXTURE_PATH, cubeTextureImage);
	}

	void FirstApp::drawFrame() {
		uint32_t imageIndex;
		auto result = lveSwapChain.acquireNextImage(&imageIndex);

		if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
			throw std::runtime_error("failed to acquire swap chain image");
		}

		updateUniformBuffer(lveSwapChain.getCurrentFrame());

		result = lveSwapChain.submitCommandBuffers(&commandBuffers[imageIndex], &imageIndex);
		if (result != VK_SUCCESS) {
			throw std::runtime_error("failed to present swap chain image");
		}
	}

	void FirstApp::loadModels() {
		model = std::make_unique<Model>(lveDevice, applicationPipelines.opaquePipeline.descriptorSetLayout, textureImage.view, textureSampler, MODEL_PATH);
		cubeModel = std::make_unique<Model>(lveDevice, applicationPipelines.opaquePipeline.descriptorSetLayout, cubeTextureImage.view, textureSampler, CUBE_MODEL_PATH);
	}

	void FirstApp::updateUniformBuffer(uint32_t currentImage) {
		static auto startTime = std::chrono::high_resolution_clock::now();

		auto currentTime = std::chrono::high_resolution_clock::now();
		float time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();

		UniformBufferObject ubo{};
		ubo.model = glm::rotate(glm::mat4(1.0f), time * glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		ubo.view = glm::lookAt(glm::vec3(2.0f, 2.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		ubo.proj = glm::perspective(glm::radians(45.0f), lveSwapChain.width() / (float)lveSwapChain.height(), 0.1f, 10.0f);
		ubo.color = glm::vec4(1.0f);
		// flip Y clip coordinate
		ubo.proj[1][1] *= -1;

		model->updateUniformBuffer(ubo, currentImage);

		ubo.model = glm::scale(glm::rotate(glm::mat4(1.0f), glm::radians(45.0f), glm::vec3(0.0f, 0.0f, 1.0f)), glm::vec3(0.5f));
		ubo.color = glm::vec4(1.0f, 1.0f, 0.0f, 0.5f);
		cubeModel->updateUniformBuffer(ubo, currentImage);
	}
}