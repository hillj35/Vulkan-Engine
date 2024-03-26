#pragma once

#include "camera.hpp"
#include "descriptor_allocator.hpp"
#include "lve_types.hpp"
#include "model.hpp"

#include <iostream>
#include <map>
#include <memory>
#include <vector>

namespace lve {
class IScene {
public:
    IScene(LveDevice &device, ApplicationPipelines pipelines, GLFWwindow *window)
        : lveDevice{device}, pipelines{pipelines}, camera{window} {}
    virtual ~IScene() = default;
    virtual void initScene() = 0;
    virtual void destroyScene() = 0;
    virtual void draw(VkCommandBuffer cmd, LveSwapChain &swapChain, int imageIndex, uint32_t currentFrame) = 0;
    virtual void showSceneGui() = 0;
    virtual void updateUniformBuffer(uint32_t currentImage, uint32_t width, uint32_t height) = 0;
    std::string getName() { return sceneName; }

protected:
    virtual void createDescriptorPool() = 0;

    std::string sceneName;
    ApplicationPipelines pipelines;
    LveDevice &lveDevice;
    DescriptorAllocator descriptorAllocator{lveDevice};
    Camera camera;

    std::map<Pipeline, std::vector<std::unique_ptr<Model>>> pipelineToModelMap;
};
} // namespace lve