#pragma once
#include <vector>
#include "../GameEntity/game_entity.h"
#include "../Render/render.h"

class GameLoop {
 private:
  bool is_running_ = false;

 public:
  void Start();
  void Stop();
  /**
   * \brief Цикл игрового движка
   * \param entities Список сущностей для обновления
   * \param render Объект рендера
   */
  void Loop(std::vector<GameEntity*> entities, Render* render);
};

void GameLoop::Start() {
  is_running_ = true;
}

void GameLoop::Stop() {
  is_running_ = false;
}

void GameLoop::Loop(std::vector<GameEntity*> entities, Render* render) {
  while (is_running_) {
    if (!entities.empty()) {
      for (auto entity : entities) {
        entity->UpdateEvent();  // Обновляем логику сущности
        render->UpdateGameEntity(entity);  // Обновляем рендер сущности
      }
    }
    is_running_ = false;
  }
}