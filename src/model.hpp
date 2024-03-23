#pragma once

#include "descriptor_allocator.hpp"
#include "lve_swap_chain.hpp"
#include "lve_types.hpp"

#include <glm/glm.hpp>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/hash.hpp>

// std
#include <array>
#include <vector>

namespace lve {
struct UniformBufferObject {
    glm::mat4 model;
    glm::mat4 view;
    glm::mat4 proj;
};

struct Vertex {
    glm::vec3 pos;
    glm::vec3 color;
    glm::vec2 texCoord;

    bool operator==(const Vertex &other) const {
        return pos == other.pos && color == other.color && texCoord == other.texCoord;
    }

    static VkVertexInputBindingDescription getBindingDescription() {
        VkVertexInputBindingDescription bindingDescription{};
        bindingDescription.binding = 0;
        bindingDescription.stride = sizeof(Vertex);
        bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
        return bindingDescription;
    }

    static std::array<VkVertexInputAttributeDescription, 3> getAttributeDescriptions() {
        std::array<VkVertexInputAttributeDescription, 3> attributeDescriptions{};
        attributeDescriptions[0].binding = 0;
        attributeDescriptions[0].location = 0;
        attributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
        attributeDescriptions[0].offset = offsetof(Vertex, pos);

        attributeDescriptions[1].binding = 0;
        attributeDescriptions[1].location = 1;
        attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
        attributeDescriptions[1].offset = offsetof(Vertex, color);

        attributeDescriptions[2].binding = 0;
        attributeDescriptions[2].location = 2;
        attributeDescriptions[2].format = VK_FORMAT_R32G32_SFLOAT;
        attributeDescriptions[2].offset = offsetof(Vertex, texCoord);
        return attributeDescriptions;
    }
};

class Model {
public:
    Model(LveDevice &device, Pipeline &pipeline, DescriptorAllocator &descriptorAllocator,
          VkImageView textureImageView, VkSampler textureSampler, std::string modelPath);
    ~Model();

    Model(const Model &) = delete;
    Model operator=(const Model &) = delete;
    Model(Model &&) = delete;
    Model &operator=(Model &&) = delete;

    void bind(VkCommandBuffer cmdBuffer, VkPipelineLayout pipelineLayout, size_t currentFrame);
    void draw(VkCommandBuffer cmdBuffer);
    void updateUniformBuffer(UniformBufferObject uniformBuffer, uint32_t currentImage);

    Pipeline getDrawPipeline() { return drawPipeline; }

private:
    void loadModel(std::string modelPath);
    void createDescriptorSets(VkImageView textureImageView, VkSampler textureSampler);
    void createIndexBuffer();
    void createVertexBuffer();
    void createUniformBuffers();

    LveDevice &lveDevice;
    std::vector<Vertex> vertices;
    std::vector<uint32_t> indices;
    uint32_t vertexCount;
    uint32_t indexCount;

    VkBuffer vertexBuffer;
    VkDeviceMemory vertexBufferMemory;
    VkBuffer indexBuffer;
    VkDeviceMemory indexBufferMemory;
    Pipeline &drawPipeline;
    DescriptorAllocator &descriptorAllocator;

    std::vector<VkDescriptorSet> descriptorSets;
    std::vector<VkBuffer> uniformBuffers;
    std::vector<VkDeviceMemory> uniformBuffersMemory;
    std::vector<void *> uniformBuffersMapped;
};
} // namespace lve

namespace std {
template <> struct hash<lve::Vertex> {
    size_t operator()(lve::Vertex const &vertex) const {
        return ((hash<glm::vec3>()(vertex.pos) ^ (hash<glm::vec3>()(vertex.color) << 1)) >> 1) ^
               (hash<glm::vec2>()(vertex.texCoord) << 1);
    }
};
} // namespace std