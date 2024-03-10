#include "pipelines.hpp"

#include <vector>
#include <array>
#include <stdexcept>

namespace init {
    void createPipelines(VkDevice device, lve::LveSwapChain* swapChain, lve::ApplicationPipelines* outPipelines) {
        // opaque pipeline
        // descriptor sets
        VkDescriptorSetLayoutBinding uboLayoutBinding{};
		uboLayoutBinding.binding = 0;
		uboLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		uboLayoutBinding.descriptorCount = 1;
		uboLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
		uboLayoutBinding.pImmutableSamplers = nullptr;

		VkDescriptorSetLayoutBinding samplerLayoutBinding{};
		samplerLayoutBinding.binding = 1;
		samplerLayoutBinding.descriptorCount = 1;
		samplerLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		samplerLayoutBinding.pImmutableSamplers = nullptr;
		samplerLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

		std::array<VkDescriptorSetLayoutBinding, 2> bindings = {uboLayoutBinding, samplerLayoutBinding};

		VkDescriptorSetLayoutCreateInfo layoutInfo{};
		layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
		layoutInfo.bindingCount = static_cast<uint32_t>(bindings.size());
		layoutInfo.pBindings = bindings.data();

        VkDescriptorSetLayout descriptorSetLayout;
		if (vkCreateDescriptorSetLayout(device, &layoutInfo, nullptr, &descriptorSetLayout) != VK_SUCCESS) {
			throw std::runtime_error("failed to create descriptor set layout!");
		}

        // pipeline layout
        VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
		pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		pipelineLayoutInfo.setLayoutCount = 1;
		pipelineLayoutInfo.pSetLayouts = &descriptorSetLayout;
		pipelineLayoutInfo.pushConstantRangeCount = 0;
		pipelineLayoutInfo.pPushConstantRanges = nullptr;

        VkPipelineLayout pipelineLayout;
		if (vkCreatePipelineLayout(device, &pipelineLayoutInfo, nullptr, &pipelineLayout) != VK_SUCCESS) {
			throw std::runtime_error("failed to create pipeline layout!");
		}

        lve::PipelineBuilder pipelineBuilder;

		pipelineBuilder.pipelineLayout = pipelineLayout;
		VkShaderModule vertShaderModule = lve::PipelineBuilder::createShaderModule(device, "shaders/simple_shader.vert.spv");
		VkShaderModule fragShaderModule = lve::PipelineBuilder::createShaderModule(device, "shaders/simple_shader.frag.spv");
		pipelineBuilder.setShaders(vertShaderModule, fragShaderModule);
		pipelineBuilder.setInputTopology(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST);
		pipelineBuilder.setPolygonMode(VK_POLYGON_MODE_FILL);
		pipelineBuilder.setCullMode(VK_CULL_MODE_BACK_BIT, VK_FRONT_FACE_COUNTER_CLOCKWISE);
		pipelineBuilder.setMultisamplingNone();
		pipelineBuilder.disableBlending();
		pipelineBuilder.enableDepthTest();
		pipelineBuilder.setColorAttachmentFormat(swapChain->getSwapChainImageFormat());
		pipelineBuilder.setDepthFormat(swapChain->getSwapChainDepthFormat());

		outPipelines->opaquePipeline.pipeline = pipelineBuilder.buildPipeline(device);
        outPipelines->opaquePipeline.pipelineLayout = pipelineLayout;
		outPipelines->opaquePipeline.descriptorSetLayout = descriptorSetLayout;
		outPipelines->opaquePipeline.shaderModules = { vertShaderModule, fragShaderModule };
		outPipelines->opaquePipeline.device = device;
        // todo: transparent pipeline
    }
}