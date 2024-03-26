#include "../scene.hpp"

namespace lve {
class DemoScene : public IScene {
public:
    DemoScene(LveDevice &device, ApplicationPipelines &pipelines, GLFWwindow *window);
    ~DemoScene();
    void draw(VkCommandBuffer cmd, LveSwapChain &swapChain, int imageIndex, uint32_t currentFrame);
    void showSceneGui();
    void updateUniformBuffer(uint32_t currentImage, uint32_t width, uint32_t height);

protected:
    void initScene();
    void destroyScene();
    void createDescriptorPool();
    void loadModels();
    void loadTextureImages();

private:
    const std::string ROOM_MODEL_PATH = "resources/models/viking_room.obj";
    const std::string ROOM_TEXTURE_PATH = "resources/textures/viking_room.png";
    const std::string CUBE_MODEL_PATH = "resources/models/cube.obj";
    const std::string CUBE_TEXTURE_PATH = "resources/textures/white.png";

    TransparentPushConstants pushConstants{};

    AllocatedImage roomTextureImage;
    AllocatedImage cubeTextureImage;
    VkSampler textureSampler;
};
} // namespace lve
