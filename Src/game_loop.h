// game_loop.h
#pragma once
#include <iostream>
#include <vector>

#include "EngineData/engine_data.h"
#include "EngineError/engine_logging.h"

// managers
#include "Managers/Engine/manager_inputs.h"
#include "Managers/Engine/manager_physics.h"
#include "Managers/Engine/manager_render.h"
#include "Managers/Engine/manager_resourse.h"
#include "Managers/Engine/manager_scene.h"
#include "Managers/Engine/manager_settings.h"

namespace EDD {
  //
class GameLoop {
 public:
  bool is_gameloop_enabled_ = false;        // флаг активности игрового цикла
  std::vector<Managers::Base *> managers_;  // спиок менеджеров
  ManagerSettings *manager_settings_;       // менеджер настроек

 public:
  explicit GameLoop() {
    if (!Init()) {
      std::cerr << "Error: game loop initialization failed" << std::endl;
      return;
    }
  }
  ~GameLoop() {}

  // Удаление конструкторов копирования, присваивания и перемещения
  GameLoop(const GameLoop &) = delete;
  GameLoop(GameLoop &&) = delete;
  GameLoop &operator=(const GameLoop &) = delete;
  GameLoop &operator=(GameLoop &&) = delete;

 private:
  bool Init() {
    //
    manager_settings_ = new ManagerSettings();
    // INFO загрузка настроек движка
    if (!manager_settings_) {
      LOG::Fatal("manager_settings_ is null");
      return false;
      if (IsLoadViewportSettings() && IsLoadAudioSettings() &&
          IsLoadGraphicsSettings() && IsLoadInputSettings()) {
        LOG::Fatal("Failed to load settings");
        return false;
      }
    }

    // инициализация менеджеров
    managers_.push_back(new Managers::Inputs());
    managers_.push_back(new Managers::Entity());
    managers_.push_back(new Managers::Resourse());
    managers_.push_back(new Managers::ManagerRender());
    managers_.push_back(
        new Managers::Scene(Data::Viewport{"main", 800, 600},
                            dynamic_cast<Managers::Inputs *>(managers_[0]),  //
                            &is_gameloop_enabled_));

    if (managers_.empty()) {
      LOG::Fatal("managers list is empty");
      return false;
    }

    if (!manager_settings_) {
      LOG::Fatal("manager_settings_ is null");
      return false;
    }

    for (auto &manager : managers_) {
      manager->Init();
    }

    // TODO ввести проверку на успешную инициализацию
    return true;
  }
  void EngineLoop() {
    while (is_gameloop_enabled_) {
      // обновление всех менеджеров

      // TODO пока однопоточное , потом сделать параллельное переходящее в
      // однопоточное , но обрабатывающее только изменённые данные

      for (auto &&manager : managers_) {
        manager->Update();
      }
    }
    for (auto &manager : managers_) {
      manager->FreeResources();
    }
  }
  // INFO методы для загрузки и сохранения настроек
  bool IsLoadViewportSettings() {
    return manager_settings_->LoadSettings("Init/viewport_settings.json");
  }
  bool IsSaveViewportSettings() {
    return manager_settings_->SaveSettings("Init/viewport_settings.json");
  }
  bool IsLoadAudioSettings() {
    return manager_settings_->LoadSettings("Init/audio_settings.json");
  }
  bool IsSaveAudioSettings() {
    return manager_settings_->SaveSettings("Init/audio_settings.json");
  }
  bool IsLoadGraphicsSettings() {
    return manager_settings_->LoadSettings("Init/graphics_settings.json");
  }
  bool IsSaveGraphicsSettings() {
    return manager_settings_->SaveSettings("Init/graphics_settings.json");
  }
  bool IsLoadInputSettings() {
    return manager_settings_->LoadSettings("Init/input_settings.json");
  }
  bool IsSaveInputSettings() {
    return manager_settings_->SaveSettings("Init/input_settings.json");
  }

 public:
  void StartLoop() {
    is_gameloop_enabled_ = true;
    EngineLoop();
  }
  void StopLoop() { is_gameloop_enabled_ = false; }

  void LoadSettings() {}
};
}  // namespace EDD