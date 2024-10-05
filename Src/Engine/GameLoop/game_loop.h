#pragma once
#include <vector>
#include "../EngineEntity/Window/vieport.h"
#include "../EngineEntity/Window/window.h"
#include "../EngineEntity/engine_entity.h"
#include "../GameEntity/game_entity.h"
#include "../Render/render.h"

class GameLoop {
 private:
  bool is_running_ = false;
  GameVieport* vieport_;  // объект окна

 public:
  GameLoop() { vieport_ = new GameVieport(); }
  ~GameLoop() { delete vieport_; }

  /**
   * \brief Запуск цикла игрового движка
   */
  void Start();

  /**
   * \brief Остановка цикла игрового движка
   */
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

    vieport_->Update();
    is_running_ = false;
  }
}