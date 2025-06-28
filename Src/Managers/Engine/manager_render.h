#pragma once
// Менеджер рендера игровых объектов в сцене
#include <SFML/Graphics.hpp>
#include <vector>

#include "Engine/delegate.h"
#include "Engine/interface.h"
#include "EngineData/engine_entity.h"
//

#include "manager_base.h"
namespace EDD::Managers {

class ManagerRender : public Managers::Base {
 private:
  sf::RenderWindow* window_ = nullptr;

 public:
  ManagerRender() {
    Init();
  }
  ~ManagerRender() {
    FreeResources();
  }

  /**
   * @brief Обновление рендера (в тике)
   *
   */
  virtual void Update() override {
    if (!window_) {
      return;
    }
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
};
}  // namespace EDD::Managers