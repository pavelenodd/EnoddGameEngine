// game_loop.h
#pragma once
#include <SFML/Graphics.hpp>
#include <algorithm>
#include <string>
#include <unordered_map>
//
// #include "EngineData/engine_data.h"
#include "EngineError/engine_logging.h"

// managers
#include "Managers/manager_entity.h"
#include "Managers/manager_inputs.h"
#include "Managers/manager_physics.h"
#include "Managers/manager_render.h"
#include "Managers/manager_resource.h"
#include "Managers/manager_scene.h"
#include "Managers/manager_settings.h"
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

  // TODO: надо перевести в пулл объектов
  //  список менеджеров
  std::unordered_map<std::string, Managers::Base *> managers_;
  ManagerSettings *manager_settings_;  // менеджер настроек

 public:
  explicit GameLoop() {
    if (!Init()) {
      LOG::Fatal(__FILE__, __LINE__) << "game loop initialization failed";
      abort();
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
      abort();
      if (manager_settings_->LoadSettings("Init/viewport_settings.json") &&
          manager_settings_->LoadSettings("Init/audio_settings.json") &&
          manager_settings_->LoadSettings("Init/graphics_settings.json") &&
          manager_settings_->LoadSettings("Init/input_settings.json")) {
      } else {
        LOG::Fatal(__FILE__, __LINE__) << "Failed to load settings";
        abort();
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
      abort();
    }

    if (!manager_settings_) {
      LOG::Fatal(__FILE__, __LINE__) << "manager_settings_ is null";
      abort();
    }

    // Инициализация всех менеджеров
    {
      managers_["scene"]->Init();
      managers_["inputs"]->Init(
          {static_cast<Managers::Scene *>(managers_["scene"])->GetWindowRef()});
      managers_["render"]->Init(
          {static_cast<Managers::Scene *>(managers_["scene"])->GetWindowRef(),
           static_cast<Managers::Entity *>(managers_["entity"]),
           Managers::RenderType::RENDER_2D});
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
    // Создание квадратного объекта с текстурой test.png
    {
      auto *entity_mgr = static_cast<Managers::Entity *>(managers_.at("entity"));
      auto *res_mgr = static_cast<Managers::Resource *>(managers_.at("resource"));
      auto *scene_mgr = static_cast<Managers::Scene *>(managers_.at("scene"));

      sf::Texture *tex = nullptr;
      try {
        tex = res_mgr->GetTexture("test");  // имя берётся из файла test.png
      } catch (const std::exception &e) {
        LOG::Fatal(__FILE__, __LINE__) << "Texture 'test' not found: " << e.what();
      }

      if (tex) {
        auto e = entity_mgr->CreateEntity("TexturedQuad");

        auto sizeU = tex->getSize();
        sf::Vector2f size(static_cast<float>(sizeU.x), static_cast<float>(sizeU.y));
        // Можно сделать поменьше, если текстура большая
        if (size.x > 256.f || size.y > 256.f) {
          const float scale = 256.f / std::max(size.x, size.y);
          size.x *= scale;
          size.y *= scale;
        }

        auto &rect = entity_mgr->AddComponent<sf::RectangleShape>(e, size);
        rect.setTexture(tex);
        rect.setOrigin(sf::Vector2f{size.x * 0.5f, size.y * 0.5f});

        auto *window = scene_mgr->GetWindowRef();
        if (window) {
          auto winSize = window->getSize();
          rect.setPosition(sf::Vector2f{static_cast<float>(winSize.x) * 0.5f,
                                        static_cast<float>(winSize.y) * 0.5f});
        }
      }
    }
    //=====================================================================
#ifdef DEBUG
    // INFO место тестов
    {
      test_manager_inputs_ = new TestManagerInputs(
          static_cast<Managers::Inputs *>(managers_.at("inputs")));
      static_cast<Managers::Inputs *>(managers_.at("inputs"))
          ->Subscribe(test_manager_inputs_);

      auto entity = static_cast<Managers::Entity *>(managers_.at("entity"));
      entity->CreateEntity("TestEntity");
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
  Managers::Inputs *GetInputsManager() {
    return static_cast<Managers::Inputs *>(managers_.at("inputs"));
  }
  Managers::Entity *GetEntityManager() {
    return static_cast<Managers::Entity *>(managers_.at("entity"));
  }
  Managers::Physics *GetPhysicsManager() {
    return static_cast<Managers::Physics *>(managers_.at("physics"));
  }
  Managers::Resource *GetResourceManager() {
    return static_cast<Managers::Resource *>(managers_.at("resource"));
  }
  Managers::Render *GetRenderManager() {
    return static_cast<Managers::Render *>(managers_.at("render"));
  }
  Managers::Scene *GetSceneManager() {
    return static_cast<Managers::Scene *>(managers_.at("scene"));
  }
};
}  // namespace EDD