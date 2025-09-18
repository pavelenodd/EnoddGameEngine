// game_loop.h
#pragma once
#include <SFML/Graphics.hpp>
#include <any>
#include <string>
#include <tuple>
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
#include "../Tests/test_manager_render.h"
#include "../Tests/test_manager_resources.h"
#include "../Tests/test_manager_scene.h"
#include "../Tests/test_manager_settings.h"
namespace EDD {
class GameLoop {
 private:
  //======================================================================
#ifdef DEBUG
  Tests::TestManagerInputs *test_manager_inputs_;  // тесты менеджера ввода
  Tests::TestManagerScene *test_manager_scene_;    // тесты менеджера сцены
  Tests::TestManagerSettings *test_manager_settings_;    // тесты менеджера настроек
  Tests::TestManagerResources *test_manager_resources_;  // тесты менеджера ресурсов
  Tests::TestManagerRender *test_manager_render_;        // тесты менеджера рендеринга
#endif
  //======================================================================
 public:
  bool is_gameloop_enabled_ = false;  // флаг активности игрового цикла

  // TODO: надо перевести в пулл объектов
  //  список менеджеров
  std::unordered_map<std::string, Managers::Base *> managers_;
  Managers::Settings *manager_settings_;  // менеджер настроек

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
    //=====================================================================
#ifdef DEBUG
    // INFO место тестов в симуляции работы менеджера
    // {
    //   test_manager_inputs_ = new Tests::TestManagerInputs();
    //   test_manager_scene_ = new Tests::TestManagerScene();
    //   test_manager_settings_ = new Tests::TestManagerSettings();
    //   test_manager_resources_ = new Tests::TestManagerResources();
    //   test_manager_render_ = new Tests::TestManagerRender();

    //   if (!test_manager_settings_->RunTests() ||  //
    //       !test_manager_scene_->RunTests()
    //       // ||     //
    //       // !test_manager_inputs_->RunTests() ||     //
    //       // !test_manager_resources_->RunTests() ||  //
    //       // !test_manager_render_->RunTests()        //
    //   ) {
    //     abort();
    //   }
    // }
#endif

    //======================================================================
    // Зона создания менеджеров
    manager_settings_ = new Managers::Settings();
    manager_settings_->LoadSettings(Managers::SettingsType::VIEWPORT_SETTINGS);
    managers_.emplace("entity", new Managers::Entity());
    managers_.emplace("physics", new Managers::Physics());
    managers_.emplace("resource", new Managers::Resource());
    managers_.emplace("scene", new Managers::Scene());
    managers_.emplace("inputs", new Managers::Inputs());
    managers_.emplace("render", new Managers::Render());

    if (managers_.empty()) {
      LOG::Fatal(__FILE__, __LINE__) << "managers list is empty";
      abort();
    }

    if (!manager_settings_) {
      LOG::Fatal(__FILE__, __LINE__) << "manager_settings_ is null";
      abort();
    }

    // Инициализация всех менеджеров

    managers_["scene"]->Init(
        //! надо подумать как передавать параметры что бы было явно и безопасно по типизации
        //! !!!
        std::vector<std::any>{std::make_any<std::tuple<const std::string, uint16_t, uint16_t>>(
                                  std::any_cast<std::string>(manager_settings_->GetValue(
                                      Managers::SettingsType::VIEWPORT_SETTINGS, "title")),
                                  std::any_cast<int>(manager_settings_->GetValue(
                                      Managers::SettingsType::VIEWPORT_SETTINGS, "width")),
                                  std::any_cast<int>(manager_settings_->GetValue(
                                      Managers::SettingsType::VIEWPORT_SETTINGS, "height"))),
                              &is_gameloop_enabled_});
    managers_["inputs"]->Init();
    managers_["render"]->Init(
        {static_cast<Managers::Scene *>(managers_["scene"])->GetAllViewports(),
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

    // Подписска на `input` события
    {
      static_cast<Managers::Inputs *>(managers_.at("inputs"))
          ->Subscribe(static_cast<Managers::Scene *>(managers_.at("scene")));
    }
    //=====================================================================
#ifdef DEBUG
    // INFO место тестов
    // {
    //   test_manager_settings_->SetManager(manager_settings_);
    //   test_manager_scene_->SetManager(static_cast<Managers::Scene
    //   *>(managers_.at("scene")));
    //   // test_manager_inputs_->SetManager(
    //   //     static_cast<Inputs *>(managers_.at("inputs")));
    //   // test_manager_resources_->SetManager(
    //   //     static_cast<Resource *>(managers_.at("resource")));
    //   // test_manager_render_->SetManager(
    //   //     static_cast<Render *>(managers_.at("render")));

    //   if (!test_manager_settings_->RunTests() || !test_manager_scene_->RunTests() ||
    //       !test_manager_inputs_->RunTests() || !test_manager_resources_->RunTests() ||
    //       !test_manager_render_->RunTests()) {
    //     abort();
    //   }
    // }
#endif
    return true;
  }

  // TODO сделать вывод отладки у менеджеров для проверки работы
  void EngineLoop() {
    while (is_gameloop_enabled_) {
      for (auto manager : managers_) {
        manager.second->Update();
#ifdef DEBUG
        {
          // // Запуск тестов
          // if (dynamic_cast<Managers::Inputs *>(manager.second)) {
          //   test_manager_inputs_->RunTests();
          // }
          // if (dynamic_cast<Managers::Scene *>(manager.second)) {
          //   test_manager_scene_->RunTests();
          // }
        }
#endif
      }
    }

    for (auto manager : managers_) {
      manager.second->FreeResources();  // Освобождаем память
    }

    if (manager_settings_->SaveSettings(Managers::SettingsType::ALL_SETTINGS) == true) {
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