#pragma once
#include <algorithm>
#include <vector>

#include "../EngineEntity/engine_entity.h"

class Render {
 private:
 public:
  // Метод для добавления нового наблюдателя
  void Attach(GameEntity* observer);

  // Метод для удаления наблюдателя
  void Detach(GameEntity* observer);

  // Уведомляет наблюдателей о событии
  void UpdateGameEntity(GameEntity* observer);
};

void Render::UpdateGameEntity(GameEntity* observer) {
  observer->UpdateRender();
}
