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
        VkPipeline pipeline;
        VkPipelineLayout layout;
        std::vector<VkShaderModule> shaderModules;
        VkDescriptorSetLayout descriptorSetLayout;
    };
    
    struct ApplicationPipelines {
        Pipeline opaquePipeline;
        Pipeline transparentPipeline;
    };

    void destroyImage(VkDevice device, const AllocatedImage& img);
    void destroyApplicationPipelines(VkDevice device, const ApplicationPipelines& pipelines);
    void destroyPipeline(VkDevice device, const Pipeline& pipeline);
}