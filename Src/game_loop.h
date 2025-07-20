// game_loop.h
#pragma once
#include <string>
#include <unordered_map>
//
// #include "EngineData/engine_data.h"
#include "EngineError/engine_logging.h"

// managers
#include "Managers/Engine/manager_entity.h"
#include "Managers/Engine/manager_inputs.h"
#include "Managers/Engine/manager_physics.h"
#include "Managers/Engine/manager_render.h"
#include "Managers/Engine/manager_resource.h"
#include "Managers/Engine/manager_scene.h"
#include "Managers/Engine/manager_settings.h"
// tests
#include "../Tests/test_manager_inputs.h"
namespace EDD {
class GameLoop {
 private:
  //======================================================================

  TestManagerInputs *test_manager_inputs_;  // тесты менеджера ввода

  //======================================================================
 public:
  bool is_gameloop_enabled_ = false;  // флаг активности игрового цикла
  // список менеджеров
  std::unordered_map<std::string, Managers::Base *> managers_;
  ManagerSettings *manager_settings_;  // менеджер настроек

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
      if (manager_settings_->LoadSettings("Init/viewport_settings.json") &&
          manager_settings_->LoadSettings("Init/audio_settings.json") &&
          manager_settings_->LoadSettings("Init/graphics_settings.json") &&
          manager_settings_->LoadSettings("Init/input_settings.json")) {
      } else {
        LOG::Fatal("Failed to load settings");
        return false;
      }
    }

    // Зона создания менеджеров
    managers_.emplace("inputs", new Managers::Inputs());
    managers_.emplace("entity", new Managers::Entity());
    managers_.emplace("physics", new Managers::Physics());
    managers_.emplace("resource", new Managers::Resource());
    managers_.emplace("render", new Managers::Render());
    managers_.emplace("scene",
                      new Managers::Scene({"main", 800, 600}, &is_gameloop_enabled_));

    if (managers_.empty()) {
      LOG::Fatal("managers list is empty");
      return false;
    }

    if (!manager_settings_) {
      LOG::Fatal("manager_settings_ is null");
      return false;
    }

    // Инициализация всех менеджеров
    {
      managers_["scene"]->Init();
      managers_["inputs"]->Init(
          {static_cast<Managers::Scene *>(managers_["scene"])->GetWindowRef()});
    }

    // Подписска на `input` события
    {
      static_cast<Managers::Inputs *>(managers_.at("inputs"))
          ->Subscribe(static_cast<Managers::Scene *>(managers_.at("scene")));
    }
    //=====================================================================

    // зона подключения тестов

    test_manager_inputs_ = new TestManagerInputs(
        static_cast<Managers::Inputs *>(managers_.at("inputs")));

    static_cast<Managers::Inputs *>(managers_.at("inputs"))
        ->Subscribe(test_manager_inputs_);

    //======================================================================
    return true;
  }

  // TODO сделать вывод отладки у менеджеров для проверки работы
  void EngineLoop() {
    while (is_gameloop_enabled_) {
      // Запуск тестов
      // test_manager_inputs_->RunTests();

      for (auto manager : managers_) {
        manager.second->Update();
      }
    }

    for (auto manager : managers_) {
      manager.second->FreeResources();  // Освобождаем память
    }

    if (manager_settings_->SaveSettings("Init/viewport_settings.json") &&
        manager_settings_->SaveSettings("Init/audio_settings.json") &&
        manager_settings_->SaveSettings("Init/graphics_settings.json") &&
        manager_settings_->SaveSettings("Init/input_settings.json")) {
      LOG::Info("All settings saved successfully");
    } else {
      LOG::Fatal("Failed to save some settings");
    }
    manager_settings_->FreeResources();  // Освобождаем память
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