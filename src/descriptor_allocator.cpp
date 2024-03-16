#include "descriptor_allocator.hpp"

#include <stdexcept>
#include <iostream>

namespace lve {
    DescriptorAllocator::DescriptorAllocator(LveDevice& device) : device { device } {
        std::cout << "descriptor allocator constructor\n";
    }

    DescriptorAllocator::~DescriptorAllocator() {
        std::cout << "descriptor allocator destructor\n";
        if (descriptorPool != VK_NULL_HANDLE) {
            vkDestroyDescriptorPool(device.device(), descriptorPool, nullptr);
        }
    }

    void DescriptorAllocator::createDescriptorPool(std::vector<VkDescriptorPoolSize> poolSizes, uint32_t maxSets) {
        VkDescriptorPoolCreateInfo poolInfo{};
		poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
		poolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
		poolInfo.pPoolSizes = poolSizes.data();
		poolInfo.maxSets = maxSets;

		if (vkCreateDescriptorPool(device.device(), &poolInfo, nullptr, &descriptorPool) != VK_SUCCESS) {
			throw std::runtime_error("failed to create descriptor pool");
		}
    }

    void DescriptorAllocator::allocateDescriptorSets(VkDescriptorSetLayout layout, std::vector<VkDescriptorSet>& outDescriptorSets) {
        std::vector<VkDescriptorSetLayout> layouts(LveSwapChain::MAX_FRAMES_IN_FLIGHT, layout);
		VkDescriptorSetAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
		allocInfo.descriptorPool = descriptorPool;
		allocInfo.descriptorSetCount = static_cast<uint32_t>(LveSwapChain::MAX_FRAMES_IN_FLIGHT);
		allocInfo.pSetLayouts = layouts.data();

        outDescriptorSets.resize(LveSwapChain::MAX_FRAMES_IN_FLIGHT);
        if (vkAllocateDescriptorSets(device.device(), &allocInfo, outDescriptorSets.data()) != VK_SUCCESS) {
			throw std::runtime_error("failed to allocate descriptor sets");
		}
    }
}