#pragma once
// Менеджер рендера игровых объектов в сцене
#include <SFML/Graphics.hpp>
#include <iostream>
#include <vector>

#include "../../EngineData/Entity/coord.h"
#include "Engine/delegate.h"
#include "Engine/interface.h"
#include "EngineData/engine_entity.h"
//

#include "manager_base.h"
#include "manager_entity.h"

namespace EDD::Managers {
/*
  Менеджер рендера отвечает за отрисовку игровых объектов на экране.
      Устанавливает окно для рендеринга
      принимает события от менеджера ввода(Inputs)
      принимает ресурсы от менеджера ресурсов(Resource)
*/
class Render : public Managers::Base {
 private:
  sf::RenderWindow* window_ = nullptr;
  Entity* entity_manager_ = nullptr;

 public:
  Render() {
    Init();
  }
  ~Render() {
    FreeResources();
  }

  /**
   * @brief Обновление рендера (в тике)
   *
   */
  virtual void Update() override {
    if (!window_ || !entity_manager_) { return; }
  };

  /**
   * @brief Инициализация менеджера (При старте)
   */
  virtual void Init() override {};

  /**
   * @brief Освобождение ресурсов
   */
  virtual void FreeResources() override {

  };

  /**
   * @brief Установка окна для рендеринга
   */
  void SetWindow(sf::RenderWindow* window) {
    window_ = window;
  }

 private:
};
}  // namespace EDD::Managers