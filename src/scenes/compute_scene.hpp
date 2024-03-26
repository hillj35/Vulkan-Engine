#include "../scene.hpp"

namespace lve {
class ComputeScene : public IScene {
public:
    ComputeScene(LveDevice &device, ApplicationPipelines &pipelines, GLFWwindow *window);
    ~ComputeScene();
    void initScene();
    void destroyScene();
    void draw(VkCommandBuffer cmd, LveSwapChain &swapChain, int imageIndex, uint32_t currentFrame);
    void showSceneGui();
    void updateUniformBuffer(uint32_t currentImage, uint32_t width, uint32_t height);

protected:
    virtual void createDescriptorPool();
    void createDescriptorSets();
    void createComputeImages();

private:
    const int width = 1600;
    const int height = 900;

    PerlinPushConstants pushConstants{};
    std::vector<AllocatedImage> computeImages;
    std::vector<VkDescriptorSet> computeDescriptorSets;
};
} // namespace lve