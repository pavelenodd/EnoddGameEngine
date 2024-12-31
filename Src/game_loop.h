// game_loop.h
#pragma once
#include <iostream>
#include <vector>

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
    managers_.push_back(new ManagerInputs());
    managers_.push_back(new ManagerPhysics());
    managers_.push_back(new ManagerResourse());
    managers_.push_back(new ManagerScene());
  }
  ~GameLoop() {}

 private:
  void EngineLoop() {
    while (is_gameloop_enabled_) {
      // обновление всех менеджеров
      for (auto& manager : managers_) {
        manager->Update();
      }
    }
  }

 public:
  void StartLoop() { is_gameloop_enabled_ = true; }
  void StopLoop() { is_gameloop_enabled_ = false; }
};