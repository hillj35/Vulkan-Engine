#include "lve_types.hpp"

namespace lve {
void destroyImage(VkDevice device, const AllocatedImage &img) {
    vkDestroyImageView(device, img.view, nullptr);
    vkDestroyImage(device, img.image, nullptr);
    vkFreeMemory(device, img.memory, nullptr);
}

void destroyApplicationPipelines(VkDevice device, const ApplicationPipelines &pipelines) {
    destroyPipeline(device, pipelines.opaquePipeline);
    vkDestroyPipeline(device, pipelines.transparentPipeline.pipeline, nullptr);
    destroyPipeline(device, pipelines.computePipelines.perlinNoisePipeline);
}

void destroyPipeline(VkDevice device, const Pipeline &pipeline) {
    vkDestroyPipeline(device, pipeline.pipeline, nullptr);
    vkDestroyPipelineLayout(device, pipeline.layout, nullptr);
    vkDestroyDescriptorSetLayout(device, pipeline.descriptorSetLayout, nullptr);
    for (VkShaderModule shader : pipeline.shaderModules) {
        vkDestroyShaderModule(device, shader, nullptr);
    }
}
} // namespace lve