#include "scene_manager.hpp"
#include "scenes/demo_scene.hpp"
#include "scenes/compute_scene.hpp"

#include "imgui.h"

namespace lve {
    SceneManager::SceneManager(LveDevice& device, ApplicationPipelines& pipelines) : device { device }, pipelines { pipelines } {
        initScenes();
    }

    SceneManager::~SceneManager() {
        currentScene->destroyScene();
    }

    void SceneManager::changeScene() {
        if (currentScene) {
            currentScene->destroyScene();
            currentScene.reset();
        }
        currentScene = scenes[sceneChangeIdx];
        currentScene->initScene();
        _shouldChangeScene = false;
    }

    void SceneManager::showSceneSelectGui() {
        ImGui::Begin("Scene Select");
        if (ImGui::BeginCombo("Scene Select", currentScene->getName().c_str())) {
            for (int i = 0; i < scenes.size(); i++) {
                if (ImGui::Selectable(scenes[i]->getName().c_str(), scenes[i] == currentScene)) {
                    sceneChangeIdx = i;
                    _shouldChangeScene = true;
                }
            }
            ImGui::EndCombo();
        }

        ImGui::End();
    }

    void SceneManager::initScenes() {
        scenes.push_back(std::make_shared<DemoScene>(device, pipelines));
        scenes.push_back(std::make_shared<ComputeScene>(device, pipelines));
        changeScene();
    }
}