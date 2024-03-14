#include "scene_manager.hpp"

#include "imgui.h"

namespace lve {
    SceneManager::SceneManager(LveDevice& device, ApplicationPipelines& pipelines) : device { device }, pipelines { pipelines } {
        initScenes();
    }

    SceneManager::~SceneManager() {
        
    }

    void SceneManager::showSceneSelectGui() {
        ImGui::ShowDemoWindow();
    }

    void SceneManager::initScenes() {
        scenes.push_back(std::make_shared<DemoScene>(device, pipelines));

        currentScene = scenes[0];
    }
}