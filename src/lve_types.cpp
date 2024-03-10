#include "lve_types.hpp"

namespace lve {
    void destroyImage(VkDevice device, const AllocatedImage& img) {
        vkDestroyImageView(device, img.view, nullptr);
        vkDestroyImage(device, img.image, nullptr);
        vkFreeMemory(device, img.memory, nullptr);
    }
}