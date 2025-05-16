// game_loop.h
#pragma once
#include <iostream>
#include <vector>

#include "EngineData/engine_data.h"
#include "EngineError/engine_logging.h"

// managers
#include "Managers/manager_inputs.h"
#include "Managers/manager_physics.h"
#include "Managers/manager_resourse.h"
#include "Managers/manager_scene.h"
namespace EDD {
class GameLoop {
public:
  bool is_gameloop_enabled_ = false;       // флаг активности игрового цикла
  std::vector<Managers::Base *> managers_; // спиок менеджеров

public:
  GameLoop() {
    if (!Init()) {
      std::cerr << "Error: game loop initialization failed" << std::endl;
      return;
    }
  }
  ~GameLoop() {}

private:
  bool Init() {
    SDL_Init(SDL_INIT_VIDEO);

    // инициализация менеджеров
    managers_.push_back(new Managers::Inputs());
    managers_.push_back(new Managers::Physics());
    managers_.push_back(new Managers::Resourse());
    managers_.push_back(
        new Managers::Scene(Data::Viewport{"main", 800, 600},
                            dynamic_cast<Managers::Inputs *>(managers_[0]), //
                            &is_gameloop_enabled_));

    if (managers_.empty()) {
       LOG::Critical("managers list is empty");
      return false;
    }
    LOG::Critical("managers list is empty");
    for (auto &manager : managers_) {
      manager->Init();
    }
    // TODO ввести проверку на успешную инициализацию
    return true;
  }
  void EngineLoop() {
    while (is_gameloop_enabled_) {
      // обновление всех менеджеров

      /*
      TODO пока однопоточное , потом сделать параллельное переходящее в
       однопоточное , но обрабатывающее только изменённые данные
      */

      for (auto &&manager : managers_) {
        manager->Update();
      }
    }
    for (auto &manager : managers_) {
      manager->FreeResources();
    }
  }

public:
  void StartLoop() {
    is_gameloop_enabled_ = true;
    EngineLoop();
  }
  void StopLoop() { is_gameloop_enabled_ = false; }
};
} // namespace EDD