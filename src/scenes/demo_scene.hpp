#include "../scene.hpp"

namespace lve {
    class DemoScene : public IScene {
        public:
            DemoScene(LveDevice& device, ApplicationPipelines& pipelines);
            ~DemoScene();
            void draw(VkCommandBuffer cmd, uint32_t currentFrame);
            void showSceneGui();
            void updateUniformBuffer(uint32_t currentImage, uint32_t width, uint32_t height);

        protected:
            virtual void createDescriptors();
            virtual void loadModels();
            virtual void loadTextureImages();

            std::string sceneName;
            ApplicationPipelines pipelines;
            LveDevice& lveDevice;
            DescriptorAllocator descriptorAllocator { lveDevice };
            
            std::map<Pipeline, std::vector<std::unique_ptr<Model>>> pipelineToModelMap;

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
}
