#pragma once

#include "../lve_device.hpp"
#include "../lve_types.hpp"

namespace util {
    void transitionImageLayout(VkCommandBuffer commandBuffer, VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout);
    void loadTextureImage(lve::LveDevice* lveDevice, std::string texturePath, lve::AllocatedImage &outImage);
}