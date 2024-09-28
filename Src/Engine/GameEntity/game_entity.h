#pragma once

#include "../EngineEntity/engine_entity.h"

#include <iostream>

class GameEntity : public ObserverObject {
 private:
 public:
  void UpdateEvent() { std::cout << "GameEntity Update Logic" << std::endl; }

  void UpdateRender() { std::cout << "GameEntity Update Render" << std::endl; }
};
