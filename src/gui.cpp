#include "gui.hpp"

#include "initializers/initializers.hpp"

#include <array>
#include <stdexcept>

namespace lve {
    LveGui::LveGui(LveDevice& device, LveSwapChain& swapChain, LveWindow& window) : device{device}, swapChain{swapChain}, window{window} {
        init();
    }

    LveGui::~LveGui() {
        ImGui_ImplVulkan_Shutdown();
        vkDestroyDescriptorPool(device.device(), imguiPool, nullptr);
    }

    void LveGui::init() {
        VkDescriptorPoolSize poolSizes[] = { { VK_DESCRIPTOR_TYPE_SAMPLER, 1000 },
            { VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1000 },
            { VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, 1000 },
            { VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1000 },
            { VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER, 1000 },
            { VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER, 1000 },
            { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1000 },
            { VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1000 },
            { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 1000 },
            { VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, 1000 },
            { VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, 1000 } };
            
        VkDescriptorPoolCreateInfo poolInfo = { .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO };
        poolInfo.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
        poolInfo.maxSets = 1000;
        poolInfo.poolSizeCount = (uint32_t)std::size(poolSizes);
        poolInfo.pPoolSizes = poolSizes;

        if (vkCreateDescriptorPool(device.device(), &poolInfo, nullptr, &imguiPool) != VK_SUCCESS) {
            throw std::runtime_error("failed to create imgui descriptor pool");
        }

        const VkFormat format = swapChain.getSwapChainImageFormat();
        VkPipelineRenderingCreateInfo renderingInfo { .sType = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO_KHR };
        renderingInfo.colorAttachmentCount = 1;
        renderingInfo.pColorAttachmentFormats = &format;

        ImGui::CreateContext();
        ImGui_ImplGlfw_InitForVulkan(window.getWindow(), true);

        ImGui_ImplVulkan_InitInfo initInfo = {};
        initInfo.Instance = device.instance();
        initInfo.PhysicalDevice = device.physicalDevice();
        initInfo.Device = device.device();
        initInfo.Queue = device.graphicsQueue();
        initInfo.DescriptorPool = imguiPool;
        initInfo.MinImageCount = 3;
        initInfo.ImageCount = 3;
        initInfo.UseDynamicRendering = true;
        initInfo.MSAASamples = VK_SAMPLE_COUNT_1_BIT;
        initInfo.PipelineRenderingCreateInfo = renderingInfo;

        ImGui_ImplVulkan_Init(&initInfo);
        ImGui_ImplVulkan_CreateFontsTexture();
        ImGui_ImplVulkan_DestroyFontsTexture();
    }

    void LveGui::showColorPicker(float* outColor) {
        ImGui::Begin("Cube Color");
        static ImVec4 color = ImVec4(114.0f / 255.0f, 144.0f / 255.0f, 154.0f / 255.0f, 200.0f / 255.0f);
        ImGui::ColorEdit4("Cube Color", outColor);
        ImGui::End();
    }

    void LveGui::draw(VkCommandBuffer cmd, VkImageView targetImageView) {
        VkRenderingAttachmentInfo colorAttachment = init::attachmentInfo(targetImageView, nullptr, VK_IMAGE_LAYOUT_GENERAL);
        VkRenderingInfo renderInfo = init::renderingInfo(swapChain.getSwapChainExtent(), &colorAttachment, nullptr);

        vkCmdBeginRendering(cmd, &renderInfo);

        ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), cmd);

        vkCmdEndRendering(cmd);
    }
}