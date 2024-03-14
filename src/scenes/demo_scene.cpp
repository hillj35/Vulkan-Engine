#include "demo_scene.hpp"
#include "imgui.h"

#include "../utility/images.hpp"
#include "../initializers/images.hpp"

#include <ranges>
#include <chrono>
#include <iostream>

namespace lve {
    DemoScene::DemoScene(LveDevice& device, ApplicationPipelines& pipelines) : lveDevice { device }, pipelines { pipelines }, IScene {device, pipelines} {
        init::createImageSampler(lveDevice.device(), lveDevice.properties.limits.maxSamplerAnisotropy, textureSampler);
        loadTextureImages();
        loadModels();
    }

    DemoScene::~DemoScene() {
		vkDestroySampler(lveDevice.device(), textureSampler, nullptr);
		destroyImage(lveDevice.device(), roomTextureImage);
		destroyImage(lveDevice.device(), cubeTextureImage);
    }

    void DemoScene::draw(VkCommandBuffer cmd, uint32_t currentFrame) {
		TransparentPushConstants defaultPushConstants{};
		defaultPushConstants.color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);

        for (auto pipeline : std::views::keys(pipelineToModelMap)) {
            vkCmdBindPipeline(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline.pipeline);
            TransparentPushConstants* constants = pipeline.transparent ? &pushConstants : &defaultPushConstants;
            for (auto& model : pipelineToModelMap[pipeline]) {
                model->bind(cmd, pipeline.layout, currentFrame);
                vkCmdPushConstants(cmd, pipeline.layout, VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(TransparentPushConstants), constants);
                model->draw(cmd);
            }
        }
    }

    void DemoScene::showSceneGui() {
        ImGui::Begin("Cube Color");
        static ImVec4 color = ImVec4(114.0f / 255.0f, 144.0f / 255.0f, 154.0f / 255.0f, 200.0f / 255.0f);
        ImGui::ColorEdit4("Cube Color", (float*)&pushConstants.color);
        ImGui::End();
    }

    void DemoScene::updateUniformBuffer(uint32_t currentImage, uint32_t width, uint32_t height) {
		static auto startTime = std::chrono::high_resolution_clock::now();

		auto currentTime = std::chrono::high_resolution_clock::now();
		float time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();

		UniformBufferObject ubo{};
		ubo.model = glm::rotate(glm::mat4(1.0f), time * glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		ubo.view = glm::lookAt(glm::vec3(2.0f, 2.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		ubo.proj = glm::perspective(glm::radians(45.0f), (float)width / height, 0.1f, 10.0f);
		// flip Y clip coordinate
		ubo.proj[1][1] *= -1;

        for (auto pipeline : std::views::keys(pipelineToModelMap)) {
            for (auto& model : pipelineToModelMap[pipeline]) {
                model->updateUniformBuffer(ubo, currentImage);
            }
        }
	}

    void DemoScene::loadTextureImages() {
        util::loadTextureImage(&lveDevice, ROOM_TEXTURE_PATH, roomTextureImage);
		util::loadTextureImage(&lveDevice, CUBE_TEXTURE_PATH, cubeTextureImage);
    }

    void DemoScene::loadModels() {
        std::vector<std::unique_ptr<Model>> models;

        models.push_back(std::make_unique<Model>(lveDevice, pipelines.opaquePipeline, roomTextureImage.view, textureSampler, ROOM_MODEL_PATH));
        models.push_back(std::make_unique<Model>(lveDevice, pipelines.transparentPipeline, cubeTextureImage.view, textureSampler, CUBE_MODEL_PATH));

        for (auto& model : models) {
            pipelineToModelMap[model->getDrawPipeline()].push_back(std::move(model));
        }
    }
}