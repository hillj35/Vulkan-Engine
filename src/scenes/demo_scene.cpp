#include "demo_scene.hpp"
#include "imgui.h"

#include "../initializers/images.hpp"
#include "../utility/images.hpp"

#include <chrono>
#include <iostream>
#include <ranges>

namespace lve {
DemoScene::DemoScene(LveDevice &device, ApplicationPipelines &pipelines)
    : IScene{device, pipelines} {
    sceneName = "Demo Scene";
}

DemoScene::~DemoScene() {}

void DemoScene::initScene() {
    init::createImageSampler(lveDevice.device(), lveDevice.properties.limits.maxSamplerAnisotropy,
                             textureSampler);
    createDescriptorPool();
    loadTextureImages();
    loadModels();
}

void DemoScene::destroyScene() {
    descriptorAllocator.destroyDescriptorPool();
    vkDestroySampler(lveDevice.device(), textureSampler, nullptr);
    destroyImage(lveDevice.device(), roomTextureImage);
    destroyImage(lveDevice.device(), cubeTextureImage);
}

void DemoScene::createDescriptorPool() {
    std::vector<VkDescriptorPoolSize> poolSizes{};
    poolSizes.push_back({VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
                         static_cast<uint32_t>(LveSwapChain::MAX_FRAMES_IN_FLIGHT)});
    poolSizes.push_back({VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
                         static_cast<uint32_t>(LveSwapChain::MAX_FRAMES_IN_FLIGHT)});

    descriptorAllocator.createDescriptorPool(poolSizes, 10);
}

void DemoScene::draw(VkCommandBuffer cmd, LveSwapChain &swapChain, int imageIndex,
                     uint32_t currentFrame) {
    VkRenderingAttachmentInfo colorAttachment = {.sType =
                                                     VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO};
    colorAttachment.imageView = swapChain.getImageView(imageIndex);
    colorAttachment.imageLayout = VK_IMAGE_LAYOUT_GENERAL;
    colorAttachment.clearValue = {0.1f, 0.1f, 0.1f, 1.0f};
    colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;

    VkRenderingAttachmentInfo depthAttachment{.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO};
    depthAttachment.imageView = swapChain.getDepthImageView(imageIndex);
    depthAttachment.imageLayout = VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL;
    depthAttachment.clearValue.depthStencil = {1.0f, 0};
    depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    depthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;

    VkRenderingInfo renderingInfo{.sType = VK_STRUCTURE_TYPE_RENDERING_INFO};
    renderingInfo.renderArea = VkRect2D{VkOffset2D{0, 0}, swapChain.getSwapChainExtent()};
    renderingInfo.layerCount = 1;
    renderingInfo.colorAttachmentCount = 1;
    renderingInfo.pColorAttachments = &colorAttachment;
    renderingInfo.pDepthAttachment = &depthAttachment;
    renderingInfo.pStencilAttachment = nullptr;
    vkCmdBeginRendering(cmd, &renderingInfo);

    VkViewport viewport = {};
    viewport.x = 0;
    viewport.y = 0;
    viewport.width = swapChain.getSwapChainExtent().width;
    viewport.height = swapChain.getSwapChainExtent().height;
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;

    vkCmdSetViewport(cmd, 0, 1, &viewport);

    VkRect2D scissor = {};
    scissor.offset.x = 0;
    scissor.offset.y = 0;
    scissor.extent.width = swapChain.getSwapChainExtent().width;
    scissor.extent.height = swapChain.getSwapChainExtent().height;

    vkCmdSetScissor(cmd, 0, 1, &scissor);

    TransparentPushConstants defaultPushConstants{};
    defaultPushConstants.color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);

    for (auto pipeline : std::views::keys(pipelineToModelMap)) {
        vkCmdBindPipeline(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline.pipeline);
        TransparentPushConstants *constants =
            pipeline.transparent ? &pushConstants : &defaultPushConstants;
        for (auto &model : pipelineToModelMap[pipeline]) {
            model->bind(cmd, pipeline.layout, currentFrame);
            vkCmdPushConstants(cmd, pipeline.layout, VK_SHADER_STAGE_VERTEX_BIT, 0,
                               sizeof(TransparentPushConstants), constants);
            model->draw(cmd);
        }
    }

    vkCmdEndRendering(cmd);

    util::transitionImageLayout(cmd, swapChain.getImage(imageIndex),
                                swapChain.getSwapChainImageFormat(), VK_IMAGE_LAYOUT_UNDEFINED,
                                VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);
}

void DemoScene::showSceneGui() {
    ImGui::Begin("Cube Color");
    static ImVec4 color =
        ImVec4(114.0f / 255.0f, 144.0f / 255.0f, 154.0f / 255.0f, 200.0f / 255.0f);
    ImGui::ColorEdit4("Cube Color", (float *)&pushConstants.color);
    ImGui::End();
}

void DemoScene::updateUniformBuffer(uint32_t currentImage, uint32_t width, uint32_t height) {
    static auto startTime = std::chrono::high_resolution_clock::now();

    auto currentTime = std::chrono::high_resolution_clock::now();
    float time =
        std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();

    UniformBufferObject ubo{};
    ubo.model =
        glm::rotate(glm::mat4(1.0f), time * glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    ubo.view = glm::lookAt(glm::vec3(2.0f, 2.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f),
                           glm::vec3(0.0f, 0.0f, 1.0f));
    ubo.proj = glm::perspective(glm::radians(45.0f), (float)width / height, 0.1f, 10.0f);
    // flip Y clip coordinate
    ubo.proj[1][1] *= -1;

    for (auto pipeline : std::views::keys(pipelineToModelMap)) {
        for (auto &model : pipelineToModelMap[pipeline]) {
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

    models.push_back(std::make_unique<Model>(lveDevice, pipelines.opaquePipeline,
                                             descriptorAllocator, roomTextureImage.view,
                                             textureSampler, ROOM_MODEL_PATH));
    models.push_back(std::make_unique<Model>(lveDevice, pipelines.transparentPipeline,
                                             descriptorAllocator, cubeTextureImage.view,
                                             textureSampler, CUBE_MODEL_PATH));

    for (auto &model : models) {
        pipelineToModelMap[model->getDrawPipeline()].push_back(std::move(model));
    }
}
} // namespace lve