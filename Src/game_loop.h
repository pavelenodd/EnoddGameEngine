// game_loop.h
#pragma once
#include <chrono>
#include <iostream>
#include <memory>
#include <string>
#include <thread>
#include <unordered_map>
#include <vector>
//
#include "EngineData/engine_data.h"
#include "EngineError/engine_logging.h"

// managers
#include "Managers/Engine/manager_entity.h"
#include "Managers/Engine/manager_inputs.h"
#include "Managers/Engine/manager_physics.h"
#include "Managers/Engine/manager_render.h"
#include "Managers/Engine/manager_resource.h"
#include "Managers/Engine/manager_scene.h"
#include "Managers/Engine/manager_settings.h"

// test
#include <SFML/Graphics.hpp>

namespace EDD {
//
class GameLoop {
 public:
  bool is_gameloop_enabled_ = false;                            // флаг активности игрового цикла
  std::unordered_map<std::string, Managers::Base *> managers_;  // спиок менеджеров
  ManagerSettings *manager_settings_;                           // менеджер настроек

 public:
  explicit GameLoop() {
    if (!Init()) {
      LOG::Fatal("game loop initialization failed");
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
      if (IsLoadViewportSettings() && IsLoadAudioSettings() && IsLoadGraphicsSettings() && IsLoadInputSettings()) {
        LOG::Fatal("Failed to load settings");
        return false;
      }
    }

    // инициализация менеджеров
    // TODO переписать на unique_ptr
    managers_.emplace("entity", new Managers::Entity());
    managers_.emplace("physics", new Managers::Physics());
    managers_.emplace("resource", new Managers::Resource());
    managers_.emplace("render", new Managers::Render());

    // Сначала создаем менеджер ввода без окна
    managers_.emplace("inputs", new Managers::Inputs());

    // Затем создаем сцену с менеджером ввода
    managers_.emplace("scene",
                      new Managers::Scene(Data::Viewport{"main", 800, 600},
                                          static_cast<Managers::Inputs *>(managers_["inputs"]),
                                          &is_gameloop_enabled_));

    // После создания сцены устанавливаем окно для менеджера ввода
    static_cast<Managers::Inputs *>(managers_["inputs"])
        ->SetWindow(static_cast<Managers::Scene *>(managers_["scene"])->GetWindow());

    // Связывание менеджеров
    // static_cast<Managers::Scene *>(managers_["scene"])
    //     ->SetEntityManager(static_cast<Managers::Entity *>(managers_["entity"]));
    // static_cast<Managers::Scene *>(managers_["scene"])
    //     ->SetRenderManager(static_cast<Managers::Render *>(managers_["render"]));

    if (managers_.empty()) {
      LOG::Fatal("managers list is empty");
      return false;
    }

    if (!manager_settings_) {
      LOG::Fatal("manager_settings_ is null");
      return false;
    }

    for (auto &manager : managers_) {
      manager.second->Init();
    }

    // TODO ввести проверку на успешную инициализацию
    return true;
  }

  // !FIX исправить баг с двойным вызовом метода Update у Render из за связывания с Managers::Entity
  // TODO сделать вывод отладки у менеджеров для проверки работы
  void EngineLoop() {
    const float target_fps = 60.0f;
    const float frame_time = 1.0f / target_fps;
    auto last_time = std::chrono::high_resolution_clock::now();

    while (is_gameloop_enabled_) {
      auto current_time = std::chrono::high_resolution_clock::now();
      auto elapsed = std::chrono::duration<float>(current_time - last_time).count();

      if (elapsed >= frame_time) {
        // обновление всех менеджеров
        for (auto &&manager : managers_) {
          manager.second->Update();
        }

        last_time = current_time;
      }
      std::this_thread::sleep_for(std::chrono::microseconds(100));
    }

    for (auto &manager : managers_) {
      delete manager.second;     // Освобождаем память
      delete manager_settings_;  // Освобождаем память
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

  // INFO методы для управления игровым циклом
 public:
  void StartLoop() {
    is_gameloop_enabled_ = true;
    EngineLoop();
  }
  void StopLoop() {
    is_gameloop_enabled_ = false;
  }
};
}  // namespace EDD