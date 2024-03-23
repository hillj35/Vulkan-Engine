#pragma once
#include "scene.hpp"
#include <vector>

namespace lve {
class SceneManager {
public:
    SceneManager(LveDevice &device, ApplicationPipelines &pipelines);
    ~SceneManager();
    void changeScene();
    void showSceneSelectGui();
    bool shouldChangeScene() { return _shouldChangeScene; }
    std::shared_ptr<IScene> getCurrentScene() { return currentScene; }

private:
    void initScenes();

    int sceneChangeIdx = 1;
    bool _shouldChangeScene = false;
    LveDevice &device;
    ApplicationPipelines pipelines;
    std::shared_ptr<IScene> currentScene;
    std::vector<std::shared_ptr<IScene>> scenes;
};
} // namespace lve