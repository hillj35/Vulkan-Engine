#pragma once

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_vulkan.h"

#include "lve_device.hpp"
#include "lve_swap_chain.hpp"

namespace lve {
    class LveGui {
        public:
            LveGui(LveDevice& device, LveSwapChain& swapChain, LveWindow& window);
            ~LveGui();
            void init();
            void draw(VkCommandBuffer cmd, VkImageView targetImageView);
            void showColorPicker(float* outColor);

        private:
            LveDevice& device;
            LveSwapChain& swapChain;
            LveWindow& window;
    };
}