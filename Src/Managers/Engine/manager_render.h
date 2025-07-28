#pragma once
// Менеджер рендера игровых объектов в сцене
#include <SFML/Graphics.hpp>

#include "manager_base.h"
#include "manager_entity.h"

namespace EDD::Managers {
/*
  Менеджер рендера отвечает за отрисовку игровых объектов на экране.
      Рендерит в предоставленном окне менеджером сцен объекты
      >принимает объекты от менеджера сущностей (Entity)
      >принимает ресурсы от менеджера ресурсов(Resource)
      >передает объединённые данные в активное окно для отрисовки
*/
class Render : public Managers::Base {
 private:
  sf::RenderWindow* window_ = nullptr;
  Entity* entity_manager_ = nullptr;

 public:
  Render() {}
  ~Render() {
    FreeResources();
  }

  /**
   * @brief Обновление рендера (в тике)
   *
   */
  virtual void Update() override {
    if (!window_ || !entity_manager_) return;

    window_->clear(sf::Color::Black);

    entity_manager_->ForEachCoordEntity([this](entt::entity entity, Coord& coord) {
      /*
здесь должна быть логика рендера сущностей для их отрисовки в окне
      */
    });

    window_->display();
  };
  /**
   * @brief Инициализация объектов что должны быть отрисованы
   *
   */

  /**
   * @brief Инициализация менеджера (При старте)
   */
  virtual void Init(std::vector<std::any> args) override {
    LOG::Info("Render manager initialized.");

    for (const auto& arg : args) {
      if (!arg.has_value()) continue;

      if (arg.type() == typeid(sf::RenderWindow*)) {
        SetWindowRef(std::any_cast<sf::RenderWindow*>(arg));
      } else if (!entity_manager_ && arg.type() == typeid(Entity*)) {
        SetEntityManager(std::any_cast<Entity*>(arg));
      }
    }

    if (!window_) {
      LOG::Fatal("Window pointer is null or not provided in Render manager");
    }

    if (!entity_manager_) {
      LOG::Fatal("Entity manager pointer is null or not provided in Render manager");
    }

    InitRenderEntity();
  };

 private:
  /**
   * @brief Освобождение ресурсов
   */
  virtual void FreeResources() override {
    window_ = nullptr;
    entity_manager_ = nullptr;
    LOG::Info("Render manager resources freed.");
  };
  void InitRenderEntity() {
    if (!entity_manager_) {
      LOG::Fatal("EntityManager not set in Render");
      return;
    }
    LOG::Info("Initializing Render Entity");
    /*
    здесь должна быть логика инициализации
    объектов для отрисовки сведение частей сущностей для из рендера
     */
  };
  /**
   * @brief Установить окно для рендеринга
   * @param window Указатель на окно SFML
   */
  void SetWindowRef(sf::RenderWindow* window = nullptr) {
    if (window) {
      window_ = window;
    } else {
      LOG::Fatal("window is null");
    }
  }
  void SetEntityManager(Entity* entity_manager = nullptr) {
    if (entity_manager) {
      entity_manager_ = entity_manager;
    } else {
      LOG::Fatal("entity manager is null");
    }
  }

 private:
};
}  // namespace EDD::Managers