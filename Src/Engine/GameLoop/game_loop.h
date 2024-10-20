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
   * \param L_entities Список сущностей для обновления
   * \param L_render Объект рендера
   * \param L_vieport_manager Объект менеджера вьюпорта
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

void GameLoop::Loop(std::vector<GameEntity*> L_entities,
                    Render* L_render,
                    VieportManager* L_vieport_manager) {
  while (is_running_) {
    glfwPollEvents();

    // Обновление рендера
    L_render->UpdateRender();
    if (!L_entities.empty()) {
      for (auto entity : L_entities) {
        entity->UpdateEvent();  // Обновляем логику сущности
        L_render->UpdateGameEntity(
            entity);  // Уведоляем сущность об обновлении рендера
      }
      // Обновляем рендер второй раз после обновления сущностей
      L_render->UpdateRender();
    }
    // Обновляем вьюпорт
    L_vieport_manager->UpdateVieport();
  }
  is_running_ = false;
}