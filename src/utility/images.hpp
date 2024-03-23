#pragma once

#include "../lve_device.hpp"
#include "../lve_types.hpp"

namespace util {
void copyImageToImage(VkCommandBuffer commandBuffer, VkImage source, VkImage destination,
                      VkExtent2D srcSize, VkExtent2D dstSize);
void loadTextureImage(lve::LveDevice *lveDevice, std::string texturePath,
                      lve::AllocatedImage &outImage);
void transitionImageLayout(VkCommandBuffer commandBuffer, VkImage image, VkFormat format,
                           VkImageLayout oldLayout, VkImageLayout newLayout);
} // namespace util