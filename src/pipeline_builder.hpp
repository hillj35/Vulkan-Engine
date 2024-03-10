#include "lve_device.hpp"

namespace lve {


    class PipelineBuilder {
    public: 
        std::vector<VkPipelineShaderStageCreateInfo> shaderStages;

        VkPipelineInputAssemblyStateCreateInfo inputAssembly;
        VkPipelineRasterizationStateCreateInfo rasterizer;
        VkPipelineColorBlendAttachmentState colorBlendAttachment;
        VkPipelineMultisampleStateCreateInfo multisampling;
        VkPipelineLayout pipelineLayout;
        VkPipelineDepthStencilStateCreateInfo depthStencil;
        VkPipelineRenderingCreateInfo renderInfo;
        VkFormat colorAttachmentformat;

        PipelineBuilder(){ clear(); }
        static VkShaderModule createShaderModule(VkDevice device, const std::string& filePath);

        void clear();

        VkPipeline buildPipeline(VkDevice device);
        void setShaders(VkShaderModule vertexShader, VkShaderModule fragmentShader);
        void setInputTopology(VkPrimitiveTopology topology);
        void setPolygonMode(VkPolygonMode mode);
        void setCullMode(VkCullModeFlags cullMode, VkFrontFace frontFace);
        void setMultisamplingNone();
        void disableBlending();
        void setColorAttachmentFormat(VkFormat format);
        void setDepthFormat(VkFormat format);
        void disableDepthTest();
        void enableDepthTest();

    private:
        static VkPipelineShaderStageCreateInfo shaderStageCreateInfo(VkShaderStageFlagBits flags, VkShaderModule module);
    };
}