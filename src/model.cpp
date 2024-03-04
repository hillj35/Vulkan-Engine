#include "model.hpp"

#define TINYOBJLOADER_IMPLEMENTATION
#include <tiny_obj_loader.h>

// std
#include <cassert>
#include <iostream>
#include <cstring>
#include <unordered_map>

namespace lve {
	Model::Model(LveDevice& device, VkDescriptorSetLayout descriptorSetLayout, VkImageView textureImageView, VkSampler textureSampler, std::string modelPath) : lveDevice{ device } {
		loadModel(modelPath);
		createVertexBuffer();
		createIndexBuffer();
		createUniformBuffers();
		createDescriptorPool();
		createDescriptorSets(descriptorSetLayout, textureImageView, textureSampler);
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
		vkCmdBindIndexBuffer(cmdBuffer, indexBuffer, 0, VK_INDEX_TYPE_UINT32);
		vkCmdBindDescriptorSets(cmdBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, 0, 1, &descriptorSets[currentFrame], 0, nullptr);
	}

	void Model::draw(VkCommandBuffer cmdBuffer) {
		vkCmdDrawIndexed(cmdBuffer, indexCount, 1, 0, 0, 0);
	}

	void Model::updateUniformBuffer(UniformBufferObject uniformBuffer, uint32_t currentImage) {
		memcpy(uniformBuffersMapped[currentImage], &uniformBuffer, sizeof(uniformBuffer));
	}

	void Model::createDescriptorSets(VkDescriptorSetLayout descriptorSetLayout, VkImageView textureImageView, VkSampler textureSampler) {
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

			VkDescriptorImageInfo imageInfo{};
			imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
			imageInfo.imageView = textureImageView;
			imageInfo.sampler = textureSampler;

			std::array<VkWriteDescriptorSet, 2> descriptorWrites{};
			descriptorWrites[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			descriptorWrites[0].dstSet = descriptorSets[i];
			descriptorWrites[0].dstBinding = 0;
			descriptorWrites[0].dstArrayElement = 0;
			descriptorWrites[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
			descriptorWrites[0].descriptorCount = 1;
			descriptorWrites[0].pBufferInfo = &bufferInfo;

			descriptorWrites[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			descriptorWrites[1].dstSet = descriptorSets[i];
			descriptorWrites[1].dstBinding = 1;
			descriptorWrites[1].dstArrayElement = 0;
			descriptorWrites[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
			descriptorWrites[1].descriptorCount = 1;
			descriptorWrites[1].pImageInfo = &imageInfo;

			vkUpdateDescriptorSets(lveDevice.device(), static_cast<uint32_t>(descriptorWrites.size()), descriptorWrites.data(), 0, nullptr);
		}
	}

	void Model::createDescriptorPool() {
		std::array<VkDescriptorPoolSize, 2> poolSizes{};
		poolSizes[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		poolSizes[0].descriptorCount = static_cast<uint32_t>(LveSwapChain::MAX_FRAMES_IN_FLIGHT);
		poolSizes[1].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		poolSizes[1].descriptorCount = static_cast<uint32_t>(LveSwapChain::MAX_FRAMES_IN_FLIGHT);

		VkDescriptorPoolCreateInfo poolInfo{};
		poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
		poolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
		poolInfo.pPoolSizes = poolSizes.data();
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

	void Model::loadModel(std::string modelPath) {
		tinyobj::attrib_t attrib;
		std::vector<tinyobj::shape_t> shapes;
		std::vector<tinyobj::material_t> materials;
		std::string warn, err;

		if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, modelPath.c_str())) {
			throw std::runtime_error(warn + err);
		}

		std::unordered_map<Vertex, uint32_t> uniqueVertices{};

		for (const auto& shape : shapes) {
			for (const auto& index : shape.mesh.indices) {
				Vertex vertex{};

				vertex.pos = {
					attrib.vertices[3 * index.vertex_index + 0],
					attrib.vertices[3 * index.vertex_index + 1],
					attrib.vertices[3 * index.vertex_index + 2]
				};

				vertex.texCoord = {
					attrib.texcoords[2 * index.texcoord_index + 0],
					1.0f - attrib.texcoords[2 * index.texcoord_index + 1]
				};

				vertex.color = {1.0f, 1.0f, 1.0f};

				if (uniqueVertices.count(vertex) == 0) {
					uniqueVertices[vertex] = static_cast<uint32_t>(vertices.size());
					vertices.push_back(vertex);
				}

				indices.push_back(uniqueVertices[vertex]);
			}
		}

		vertexCount = static_cast<uint32_t>(vertices.size());
		indexCount = static_cast<uint32_t>(indices.size());
		assert(vertexCount >= 3 && "Vertex count must be at least 3");
	}
}