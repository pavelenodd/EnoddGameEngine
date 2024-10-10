#pragma once
#include <vector>
#include "../GameEntity/EngineEntity/engine_entity.h"
#include "../GameEntity/game_entity.h"
#include "../Render/render.h"
#include "../VieportManager/vieport_manager.h"

class GameLoop {
 private:
  bool is_running_ = false;

 public:
  GameLoop() {

  }
  ~GameLoop() {}

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
   * \param vieport_manager Объект менеджкра вьюпорта
   */
  void Loop(std::vector<GameEntity*> entities,
            Render* render,
            VieportManager* vieport_manager);
};

void GameLoop::Start() {
  is_running_ = true;
}

void GameLoop::Stop() {
  is_running_ = false;
}

void GameLoop::Loop(std::vector<GameEntity*> entities,
                    Render* render,
                    VieportManager* vieport) {
  while (is_running_) {
    render->UpdateRender();
    if (!entities.empty()) {
      for (auto entity : entities) {
        entity->UpdateEvent();  // Обновляем логику сущности
        render->UpdateGameEntity(
            entity);  // Уведоляем сущность об обновлении рендера
      }
    }
    render->UpdateRender();  // Обновляем рендер
    // vieport_manager->UpdateVieport();

    // Обновляем вьюпорт
    is_running_ = false;
  }
}