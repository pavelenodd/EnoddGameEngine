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
#ifdef DEBUG
  TestManagerInputs *test_manager_inputs_;  // тесты менеджера ввода

#endif
  //======================================================================
 public:
  bool is_gameloop_enabled_ = false;  // флаг активности игрового цикла
  // список менеджеров
  std::unordered_map<std::string, Managers::Base *> managers_;
  ManagerSettings *manager_settings_;  // менеджер настроек

 public:
  explicit GameLoop() {
    if (!Init()) {
      LOG::Fatal(__FILE__, __LINE__) << "game loop initialization failed";
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
      LOG::Fatal(__FILE__, __LINE__) << "manager_settings_ is null";
      return false;
      if (manager_settings_->LoadSettings("Init/viewport_settings.json") &&
          manager_settings_->LoadSettings("Init/audio_settings.json") &&
          manager_settings_->LoadSettings("Init/graphics_settings.json") &&
          manager_settings_->LoadSettings("Init/input_settings.json")) {
      } else {
        LOG::Fatal(__FILE__, __LINE__) << "Failed to load settings";
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
      LOG::Fatal(__FILE__, __LINE__) << "managers list is empty";
      return false;
    }

    if (!manager_settings_) {
      LOG::Fatal(__FILE__, __LINE__) << "manager_settings_ is null";
      return false;
    }

    // Инициализация всех менеджеров
    {
      managers_["scene"]->Init();
      managers_["inputs"]->Init(
          {static_cast<Managers::Scene *>(managers_["scene"])->GetWindowRef()});
      managers_["render"]->Init(
          {static_cast<Managers::Scene *>(managers_["scene"])->GetWindowRef(),
           static_cast<Managers::Entity *>(managers_["entity"])});
      managers_["resource"]->Init({"Resources/Animations",
                                   "Resources/Audio",
                                   "Resources/Fonts",
                                   "Resources/Images",
                                   "Resources/Meshs",
                                   "Resources/Models",
                                   "Resources/Scripts",
                                   "Resources/Shaders",
                                   {"Resources/Textures/test.png"}});
      managers_["entity"]->Init();
      managers_["physics"]->Init();
    }

    // Подписска на `input` события
    {
      static_cast<Managers::Inputs *>(managers_.at("inputs"))
          ->Subscribe(static_cast<Managers::Scene *>(managers_.at("scene")));
    }
    //=====================================================================
#ifdef DEBUG
    // INFO место тестов
    {
      test_manager_inputs_ = new TestManagerInputs(
          static_cast<Managers::Inputs *>(managers_.at("inputs")));

      static_cast<Managers::Inputs *>(managers_.at("inputs"))
          ->Subscribe(test_manager_inputs_);

      auto *entity_mgr = static_cast<Managers::Entity *>(managers_.at("entity"));
      auto *resource_mgr = static_cast<Managers::Resource *>(managers_.at("resource"));

      // Создание сущности, если её нет
      auto entity = entity_mgr->CreateEntity("test");

      // Загрузка текстуры
      // resource_mgr->LoadTexture("Resources/Test/test.png");
      sf::Texture *tex = resource_mgr->GetTexture("test");

      // Добавление компонента
      auto &rect = entity_mgr->AddComponent<sf::RectangleShape>(
          entity, sf::Vector2f{800.f, 600.f});
      rect.setTexture(tex);
      rect.setPosition(sf::Vector2f(0.f, 0.f));
    }
#endif

    //======================================================================
    return true;
  }

  // TODO сделать вывод отладки у менеджеров для проверки работы
  void EngineLoop() {
    while (is_gameloop_enabled_) {
      for (auto manager : managers_) {
        manager.second->Update();
#ifdef DEBUG
        {
          // Запуск тестов
          if (dynamic_cast<Managers::Inputs *>(manager.second)) {
            test_manager_inputs_->RunTests();
          }
        }
#endif
      }
    }

    for (auto manager : managers_) {
      manager.second->FreeResources();  // Освобождаем память
    }

    if (manager_settings_->SaveSettings("Init/viewport_settings.json") &&
        manager_settings_->SaveSettings("Init/audio_settings.json") &&
        manager_settings_->SaveSettings("Init/graphics_settings.json") &&
        manager_settings_->SaveSettings("Init/input_settings.json")) {
      LOG::Info(__FILE__) << "Settings saved successfully";
    } else {
      LOG::Fatal(__FILE__, __LINE__) << "Failed to save some settings";
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