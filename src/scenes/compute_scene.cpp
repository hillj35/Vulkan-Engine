#include "compute_scene.hpp"
#include "../initializers/images.hpp"
#include "../utility/images.hpp"

namespace lve {
    ComputeScene::ComputeScene(LveDevice& device, ApplicationPipelines& pipelines) 
        : IScene {device, pipelines} {
        sceneName = "Compute Preview Scene";
    }

    ComputeScene::~ComputeScene() {
    }

    void ComputeScene::initScene() {
        createComputeImages();
        createDescriptorPool();
        createDescriptorSets();
    }

    void ComputeScene::destroyScene() {
        descriptorAllocator.destroyDescriptorPool();

        for (AllocatedImage image : computeImages) {
            destroyImage(lveDevice.device(), image);
        }
    }

    void ComputeScene::createComputeImages() {
        for (int i = 0; i < LveSwapChain::MAX_FRAMES_IN_FLIGHT; i++) {
            AllocatedImage image;
            init::createImage(&lveDevice, width, height, VK_FORMAT_B8G8R8A8_UNORM, VK_IMAGE_TILING_LINEAR, VK_IMAGE_USAGE_STORAGE_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT, 0, image);
            computeImages.push_back(image);
        }
    }

    void ComputeScene::createDescriptorPool() {
        std::vector<VkDescriptorPoolSize> poolSizes {};
        poolSizes.push_back({VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, static_cast<uint32_t>(LveSwapChain::MAX_FRAMES_IN_FLIGHT)});

        descriptorAllocator.createDescriptorPool(poolSizes, 10);
    }

    void ComputeScene::createDescriptorSets() {
        descriptorAllocator.allocateDescriptorSets(pipelines.computePipelines.testPipeline.descriptorSetLayout, computeDescriptorSets);

        for (size_t i = 0; i < LveSwapChain::MAX_FRAMES_IN_FLIGHT; i++) {
            VkDescriptorImageInfo imageInfo{};
            imageInfo.imageLayout = VK_IMAGE_LAYOUT_GENERAL;
            imageInfo.imageView = computeImages[i].view;
            imageInfo.sampler = nullptr;

            std::vector<VkWriteDescriptorSet> descriptorWrites {};
            VkWriteDescriptorSet imageWrite { .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET };
            imageWrite.dstSet = computeDescriptorSets[i];
            imageWrite.dstBinding = 0;
            imageWrite.dstArrayElement = 0;
            imageWrite.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
            imageWrite.descriptorCount = 1;
            imageWrite.pImageInfo = &imageInfo;
            descriptorWrites.push_back(imageWrite);

            vkUpdateDescriptorSets(lveDevice.device(), static_cast<uint32_t>(descriptorWrites.size()), descriptorWrites.data(), 0, nullptr);
        }
    }

    void ComputeScene::draw(VkCommandBuffer cmd, LveSwapChain& swapChain, int imageIndex,  uint32_t currentFrame) {
        util::transitionImageLayout(cmd, computeImages[currentFrame].image, VK_FORMAT_B8G8R8A8_UNORM, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_GENERAL);

        vkCmdBindPipeline(cmd, VK_PIPELINE_BIND_POINT_COMPUTE, pipelines.computePipelines.testPipeline.pipeline);
        vkCmdBindDescriptorSets(cmd, VK_PIPELINE_BIND_POINT_COMPUTE, pipelines.computePipelines.testPipeline.layout, 0, 1, &computeDescriptorSets[currentFrame], 0, nullptr);
        vkCmdDispatch(cmd, std::ceil(width/16.0), std::ceil(height/16.0), 1);
        
        // copy resulting image to swapchain image
        util::transitionImageLayout(cmd, computeImages[currentFrame].image, VK_FORMAT_B8G8R8A8_UNORM, VK_IMAGE_LAYOUT_GENERAL, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL);
        util::transitionImageLayout(cmd, swapChain.getImage(imageIndex), swapChain.getSwapChainImageFormat(), VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);

        VkExtent2D srcExtent { .width = static_cast<uint32_t>(width), .height = static_cast<uint32_t>(height) };

        util::copyImageToImage(cmd, computeImages[currentFrame].image, swapChain.getImage(imageIndex), srcExtent, swapChain.getSwapChainExtent());
        
        util::transitionImageLayout(cmd, computeImages[currentFrame].image, VK_FORMAT_B8G8R8A8_UNORM, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, VK_IMAGE_LAYOUT_GENERAL);
        util::transitionImageLayout(cmd, swapChain.getImage(imageIndex), swapChain.getSwapChainImageFormat(), VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);
    }

    void ComputeScene::showSceneGui() {

    }

    void ComputeScene::updateUniformBuffer(uint32_t currentImage, uint32_t width, uint32_t height) {

    }
}