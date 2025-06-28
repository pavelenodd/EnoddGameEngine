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

class ManagerRender : public Managers::Base {
 private:
  sf::RenderWindow* window_ = nullptr;
  Entity* entity_manager_ = nullptr;

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
    if (!window_ || !entity_manager_) {
      return;
    }

    // Рендерим все сущности с координатами
    RenderCoordinates();
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

  /**
   * @brief Установка менеджера сущностей
   */
  void SetEntityManager(Entity* entity_manager) {
    entity_manager_ = entity_manager;
  }

 private:
  /**
   * @brief Рендеринг всех сущностей с координатами как точек
   */
  void RenderCoordinates() {
    if (!entity_manager_)
      return;

    int count = 0;
    entity_manager_->ForEachCoordEntity([this, &count](entt::entity entity, Coord& coord) {
      // Рендерим как простую точку (круг радиусом 100 пикселей)
      sf::CircleShape point(100.0f);
      point.setPosition(coord.position_.x - 100.0f, coord.position_.y - 100.0f);
      point.setFillColor(coord.color_);
      window_->draw(point);
      count++;
    });

    // Отладочная информация (выводим только раз в секунду)
    std::cout << "Rendering " << count << " coordinate entities" << std::endl;
  }
};
}  // namespace EDD::Managers