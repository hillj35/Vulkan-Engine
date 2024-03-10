#pragma once

#include "../lve_device.hpp"
#include "../lve_types.hpp"

namespace init {
    void createImage(lve::LveDevice* device, uint32_t width, uint32_t height, VkFormat format, VkImageTiling tiling, 
		VkImageUsageFlags usage, VkMemoryPropertyFlags properties, lve::AllocatedImage &image);
    void createImageSampler(VkDevice device, float maxAnisotropy, VkSampler &outTextureSampler);
}