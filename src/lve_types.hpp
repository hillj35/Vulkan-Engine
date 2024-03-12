#pragma once

#include <vulkan/vulkan.h>
#include <glm/glm.hpp>

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

    struct TransparentPushConstants {
        glm::mat4 transform;
        glm::vec4 color;
    };

    void destroyImage(VkDevice device, const AllocatedImage& img);
    void destroyApplicationPipelines(VkDevice device, const ApplicationPipelines& pipelines);
    void destroyPipeline(VkDevice device, const Pipeline& pipeline);
}