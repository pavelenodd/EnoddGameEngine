// game_loop.h
#pragma once
#include <iostream>
#include <vector>

#include "EngineData/engine_data.h"

// managers
#include "EngineError/engine_error.h"
#include "Managers/manager_inputs.h"
#include "Managers/manager_physics.h"
#include "Managers/manager_resourse.h"
#include "Managers/manager_scene.h"

class GameLoop {
 public:
  bool is_gameloop_enabled_ = false;    // флаг активности игрового цикла
  std::vector<ManagerBase*> managers_;  // спиок менеджеров

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
    managers_.push_back(new ManagerInputs());
    managers_.push_back(new ManagerPhysics());
    managers_.push_back(new ManagerResourse());
    managers_.push_back(new ManagerScene(EDD::ViewportData{"main", 800, 600}));

    if (managers_.empty()) {
      std::cerr << "Error: managers list is empty" << std::endl;
      EDD::errors.push_back(EDD::EngineErrorNS::code1);
      return false;
    }

    for (auto& manager : managers_) {
      manager->Init();
    }
    // TODO ввести проверку на успешную инициализацию
    return true;
  }
  void EngineLoop() {
    while (is_gameloop_enabled_) {
      // обновление всех менеджеров
      {
        // TODO пока однопоточное , потом сделать параллельное переходящее в
        // однопоточное , но обрабатывающее только изменённые данные
      }
      for (auto&& manager : managers_) {
        manager->Update();
      }
    }
  }

 public:
  void StartLoop() {
    is_gameloop_enabled_ = true;
    EngineLoop();
  }
  void StopLoop() { is_gameloop_enabled_ = false; }
};