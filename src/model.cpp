#include "model.hpp"

// std
#include <cassert>
#include <iostream>
#include <cstring>

namespace lve {
	Model::Model(LveDevice& device, VkDescriptorSetLayout descriptorSetLayout) : lveDevice{ device } {
		loadModel();
		createVertexBuffer();
		createIndexBuffer();
		createUniformBuffers();
		createDescriptorPool();
		createDescriptorSets(descriptorSetLayout);
	}

	Model::~Model() {
		vkDestroyBuffer(lveDevice.device(), vertexBuffer, nullptr);
		vkFreeMemory(lveDevice.device(), vertexBufferMemory, nullptr);
		vkDestroyBuffer(lveDevice.device(), indexBuffer, nullptr);
		vkFreeMemory(lveDevice.device(), indexBufferMemory, nullptr);
		vkDestroyDescriptorPool(lveDevice.device(), descriptorPool, nullptr);

		for (size_t i = 0; i < LveSwapChain::MAX_FRAMES_IN_FLIGHT; i++) {
			vkDestroyBuffer(lveDevice.device(), uniformBuffers[i], nullptr);
			vkFreeMemory(lveDevice.device(), uniformBuffersMemory[i], nullptr);
		}
	}

	void Model::bind(VkCommandBuffer cmdBuffer, VkPipelineLayout pipelineLayout, size_t currentFrame) {
		VkBuffer vertexBuffers[] = { vertexBuffer };
		VkDeviceSize offsets[] = { 0 };
		vkCmdBindVertexBuffers(cmdBuffer, 0, 1, vertexBuffers, offsets);
		vkCmdBindIndexBuffer(cmdBuffer, indexBuffer, 0, VK_INDEX_TYPE_UINT16);
		vkCmdBindDescriptorSets(cmdBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, 0, 1, &descriptorSets[currentFrame], 0, nullptr);
	}

	void Model::draw(VkCommandBuffer cmdBuffer) {
		vkCmdDrawIndexed(cmdBuffer, indexCount, 1, 0, 0, 0);
	}

	void Model::updateUniformBuffer(UniformBufferObject uniformBuffer, uint32_t currentImage) {
		memcpy(uniformBuffersMapped[currentImage], &uniformBuffer, sizeof(uniformBuffer));
	}

	void Model::createDescriptorSets(VkDescriptorSetLayout descriptorSetLayout) {
		std::vector<VkDescriptorSetLayout> layouts(LveSwapChain::MAX_FRAMES_IN_FLIGHT, descriptorSetLayout);
		VkDescriptorSetAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
		allocInfo.descriptorPool = descriptorPool;
		allocInfo.descriptorSetCount = static_cast<uint32_t>(LveSwapChain::MAX_FRAMES_IN_FLIGHT);
		allocInfo.pSetLayouts = layouts.data();

		descriptorSets.resize(LveSwapChain::MAX_FRAMES_IN_FLIGHT);
		if (vkAllocateDescriptorSets(lveDevice.device(), &allocInfo, descriptorSets.data()) != VK_SUCCESS) {
			throw std::runtime_error("failed to allocate descriptor sets");
		}

		for (size_t i = 0; i < LveSwapChain::MAX_FRAMES_IN_FLIGHT; i++) {
			VkDescriptorBufferInfo bufferInfo{};
			bufferInfo.buffer = uniformBuffers[i];
			bufferInfo.offset = 0;
			bufferInfo.range = sizeof(UniformBufferObject);

			VkWriteDescriptorSet descriptorWrite{};
			descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			descriptorWrite.dstSet = descriptorSets[i];
			descriptorWrite.dstBinding = 0;
			descriptorWrite.dstArrayElement = 0;
			descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
			descriptorWrite.descriptorCount = 1;
			descriptorWrite.pBufferInfo = &bufferInfo;
			descriptorWrite.pImageInfo = nullptr;
			descriptorWrite.pTexelBufferView = nullptr;

			vkUpdateDescriptorSets(lveDevice.device(), 1, &descriptorWrite, 0, nullptr);
		}
	}

	void Model::createDescriptorPool() {
		VkDescriptorPoolSize poolSize{};
		poolSize.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		poolSize.descriptorCount = static_cast<uint32_t>(LveSwapChain::MAX_FRAMES_IN_FLIGHT);

		VkDescriptorPoolCreateInfo poolInfo{};
		poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
		poolInfo.poolSizeCount = 1;
		poolInfo.pPoolSizes = &poolSize;
		poolInfo.maxSets = static_cast<uint32_t>(LveSwapChain::MAX_FRAMES_IN_FLIGHT);

		if (vkCreateDescriptorPool(lveDevice.device(), &poolInfo, nullptr, &descriptorPool) != VK_SUCCESS) {
			throw std::runtime_error("failed to create descriptor pool");
		}
	}

	void Model::createIndexBuffer() {
		VkDeviceSize bufferSize = sizeof(indices[0]) * indexCount;

		VkBuffer stagingBuffer;
		VkDeviceMemory stagingBufferMemory;

		lveDevice.createBuffer(
			bufferSize,
			VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
			stagingBuffer,
			stagingBufferMemory
		);
		void* data;
		vkMapMemory(lveDevice.device(), stagingBufferMemory, 0, bufferSize, 0, &data);
		memcpy(data, indices.data(), (size_t)bufferSize);
		vkUnmapMemory(lveDevice.device(), stagingBufferMemory);

		lveDevice.createBuffer(
			bufferSize,
			VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
			VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
			indexBuffer,
			indexBufferMemory
		);

		lveDevice.copyBuffer(stagingBuffer, indexBuffer, bufferSize);

		vkDestroyBuffer(lveDevice.device(), stagingBuffer, nullptr);
		vkFreeMemory(lveDevice.device(), stagingBufferMemory, nullptr);
	}

	void Model::createVertexBuffer() {
		VkDeviceSize bufferSize = sizeof(vertices[0]) * vertexCount;

		VkBuffer stagingBuffer; 
		VkDeviceMemory stagingBufferMemory;

		lveDevice.createBuffer(
			bufferSize,
			VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
			stagingBuffer,
			stagingBufferMemory
		);
		void* data;
		vkMapMemory(lveDevice.device(), stagingBufferMemory, 0, bufferSize, 0, &data);
		memcpy(data, vertices.data(), (size_t)bufferSize);
		vkUnmapMemory(lveDevice.device(), stagingBufferMemory);

		lveDevice.createBuffer(
			bufferSize,
			VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
			VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
			vertexBuffer,
			vertexBufferMemory
		);

		lveDevice.copyBuffer(stagingBuffer, vertexBuffer, bufferSize);

		vkDestroyBuffer(lveDevice.device(), stagingBuffer, nullptr);
		vkFreeMemory(lveDevice.device(), stagingBufferMemory, nullptr);
	}

	void Model::createUniformBuffers() {
		VkDeviceSize bufferSize = sizeof(UniformBufferObject);

		uniformBuffers.resize(LveSwapChain::MAX_FRAMES_IN_FLIGHT);
		uniformBuffersMemory.resize(LveSwapChain::MAX_FRAMES_IN_FLIGHT);
		uniformBuffersMapped.resize(LveSwapChain::MAX_FRAMES_IN_FLIGHT);

		for (size_t i = 0; i < LveSwapChain::MAX_FRAMES_IN_FLIGHT; i++) {
			lveDevice.createBuffer(
				bufferSize,
				VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
				VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
				uniformBuffers[i],
				uniformBuffersMemory[i]
			);

			vkMapMemory(lveDevice.device(), uniformBuffersMemory[i], 0, bufferSize, 0, &uniformBuffersMapped[i]);
		}
	}

	void Model::loadModel() {
		vertices = {
			{{-0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}},
			{{0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}},
			{{0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}},
			{{-0.5f, 0.5f}, {1.0f, 1.0f, 1.0f}}
		};
		indices = {
			0, 1, 2, 2, 3, 0
		};

		vertexCount = static_cast<uint32_t>(vertices.size());
		indexCount = static_cast<uint32_t>(indices.size());
		assert(vertexCount >= 3 && "Vertex count must be at least 3");
	}
}