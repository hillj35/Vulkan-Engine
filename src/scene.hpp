#pragma once

#include "model.hpp"
#include "lve_types.hpp"
#include "descriptor_allocator.hpp"

#include <map>
#include <vector>
#include <memory>
#include <iostream>

namespace lve {
    class IScene {
        public:
            IScene(LveDevice& device, ApplicationPipelines pipelines) : lveDevice{device}, pipelines{pipelines} {}
            virtual ~IScene() = default;
            virtual void draw(VkCommandBuffer cmd, uint32_t currentFrame) = 0;
            virtual void showSceneGui() = 0;
            virtual void updateUniformBuffer(uint32_t currentImage, uint32_t width, uint32_t height) = 0;

        protected:
            virtual void createDescriptors() = 0;
            virtual void loadModels() = 0;
            virtual void loadTextureImages() = 0;

            std::string sceneName;
            ApplicationPipelines pipelines;
            LveDevice& lveDevice;
            //DescriptorAllocator descriptorAllocator { lveDevice };
            
            std::map<Pipeline, std::vector<std::unique_ptr<Model>>> pipelineToModelMap;
    };
}