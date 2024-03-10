#pragma once

// vulkan headers
#include <vulkan/vulkan.h>

#include <vector>

namespace lve {
    struct AllocatedImage {
        VkImage image;
        VkImageView view;
        VkDeviceMemory memory;
    };

    struct Pipeline {
        ~Pipeline() {
            vkDestroyPipeline(device, pipeline, nullptr);
            vkDestroyPipelineLayout(device, pipelineLayout, nullptr);
            vkDestroyDescriptorSetLayout(device, descriptorSetLayout, nullptr);

            for (VkShaderModule shader : shaderModules) {
                vkDestroyShaderModule(device, shader, nullptr);
            }
        }

        VkPipeline pipeline;
        VkPipelineLayout pipelineLayout;
        std::vector<VkShaderModule> shaderModules;
        VkDescriptorSetLayout descriptorSetLayout;
        VkDevice device; 
    };
    
    struct ApplicationPipelines {
        Pipeline opaquePipeline;
        // todo: Pipeline transparentPipeline;
    };

    void destroyImage(VkDevice device, const AllocatedImage& img);
}