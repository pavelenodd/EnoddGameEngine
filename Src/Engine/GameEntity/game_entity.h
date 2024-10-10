#pragma once

#include "EngineEntity/engine_entity.h"

#include <iostream>

class GameEntity : public ObserverObject {
 private:
  bool is_render_update_pending_ = false;

 public:
  void UpdateEvent() { std::cout << "GameEntity Update Logic" << std::endl; }

  void UpdateRender() {
    if (is_render_update_pending_) {
      std::cout << "GameEntity Update Render" << std::endl;
    }
  }

  /**
   * \brief Установка запроса на обновление рендера
   * \param is_update_render Значение запроса
   */
  void Set_RenderUpdatePending(bool is_update_render) {
    is_render_update_pending_ = is_update_render;
  }
};
