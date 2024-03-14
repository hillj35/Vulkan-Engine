#pragma once
#include "scenes/demo_scene.hpp"
#include <vector>

namespace lve {
    class SceneManager {
        public:
            SceneManager(LveDevice& device, ApplicationPipelines& pipelines);
            ~SceneManager();
            void showSceneSelectGui();
            std::shared_ptr<IScene> getCurrentScene() { return currentScene; }

        private:
            void initScenes();

            LveDevice& device;
            ApplicationPipelines pipelines;
            std::shared_ptr<IScene> currentScene;
            std::vector<std::shared_ptr<IScene>> scenes;
    };
}