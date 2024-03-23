#pragma once

#include <glm/glm.hpp>
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
    bool transparent = false;

    bool operator<(const Pipeline &other) const {
        return pipeline < other.pipeline && transparent <= other.transparent;
    }
};

struct ComputePipelines {
    Pipeline perlinNoisePipeline;
};

struct ApplicationPipelines {
    Pipeline opaquePipeline;
    Pipeline transparentPipeline;
    ComputePipelines computePipelines;
};

struct TransparentPushConstants {
    glm::mat4 transform;
    glm::vec4 color;
};

struct PerlinPushConstants {
    glm::vec2 offset;
    glm::float32 scale;
};

void destroyImage(VkDevice device, const AllocatedImage &img);
void destroyApplicationPipelines(VkDevice device, const ApplicationPipelines &pipelines);
void destroyPipeline(VkDevice device, const Pipeline &pipeline);
} // namespace lve