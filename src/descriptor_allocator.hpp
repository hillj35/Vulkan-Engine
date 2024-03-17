#pragma once

#include "lve_swap_chain.hpp"

namespace lve {
    class DescriptorAllocator{
        public:
            DescriptorAllocator(LveDevice& device);
            ~DescriptorAllocator();

            // Not copyable or movable
            DescriptorAllocator(const DescriptorAllocator&) = delete;
            DescriptorAllocator operator=(const DescriptorAllocator&) = delete;
            DescriptorAllocator(DescriptorAllocator&&) = delete;
            DescriptorAllocator& operator=(DescriptorAllocator&&) = delete;

            void createDescriptorPool(std::vector<VkDescriptorPoolSize> poolSizes, uint32_t maxSets);
            void allocateDescriptorSets(VkDescriptorSetLayout layout, std::vector<VkDescriptorSet>& outDescriptorSets);
            void destroyDescriptorPool();

        private:
            VkDescriptorPool descriptorPool;
            LveDevice& device;
    };
}