#pragma once

#include "../lve_swap_chain.hpp"
#include "../pipeline_builder.hpp"

namespace init {
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

    void createPipelines(VkDevice device, lve::LveSwapChain* swapChain, ApplicationPipelines* outPipelines);
}