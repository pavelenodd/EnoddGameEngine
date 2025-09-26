#pragma once
//
// Менеджер рендера (3D + 2D) на bgfx, окно — SFML

#include <bgfx/bgfx.h>
#include <bgfx/platform.h>
#include <bx/math.h>

#include <SFML/Graphics.hpp>

#include "Items/Components/comp_render2D.h"
#include "Items/Components/comp_render3D.h"
#include "engine_logging.h"
#include "manager_base.h"
#include "manager_entity.h"
#include "manager_resource.h"

namespace EDD::Managers {
enum RenderType { NONE, RENDER_2D, RENDER_3D };
/*
  Менеджер рендера отвечает за отрисовку игровых объектов на экране.
      Рендерит в предоставленном окне менеджером сцен объекты
      >принимает объекты от менеджера сущностей (Entity)
      >принимает ресурсы от менеджера ресурсов(Resource)
      >передает объединённые данные в активное окно для отрисовки
      (3D + 2D) на bgfx, окно — SFML
*/
class Render : public Managers::Base {
 private:
  RenderType render_type_ = RenderType::NONE;
  sf::RenderWindow* window_ = nullptr;
  Render2D* render_2d_ = nullptr;
  Render3D* render_3d_ = nullptr;
  Managers::Entity* entity_manager_ = nullptr;
  Managers::Resource* resource_manager_ = nullptr;

 public:
  Render() {}
  ~Render() {
    FreeResources();
  }

  void Update() override {
    if (!window_ || !entity_manager_) return;

    if (render_type_ == RENDER_2D) {
      render_2d_->Render_All(*entity_manager_);
    } else if (render_type_ == RENDER_3D) {
      // render_3d_->Render_All(*entity_manager_);
    }

    bgfx::frame();
  }
  /**
   * @brief Initialize the render manager.
   *
   * @param args references to the required managers.
   * @details - [0] Window reference (sf::RenderWindow*),
   *          - [1] Entity manager reference (Entity*),
   *          - [2] Render type (RenderType)
   */
  void Init(std::vector<std::any> args) override {
    LOG::Debug() << "Render manager initialized.";
    if (args.size() < 3) {
      LOG::Fatal(__FILE__, __LINE__) << "Render: no arguments provided";
      abort();
    }

    if (args[0].type() != typeid(sf::RenderWindow*)) {
      LOG::Fatal(__FILE__, __LINE__) << "First argument must be sf::RenderWindow*";
      abort();
    } else {
      window_ = std::any_cast<sf::RenderWindow*>(args[0]);
    }

    if (args[1].type() != typeid(Managers::Entity*)) {
      LOG::Fatal(__FILE__, __LINE__) << "Second argument must be Managers::Entity*";
      abort();
    } else {
      entity_manager_ = std::any_cast<Managers::Entity*>(args[1]);
    }

    if (args[2].type() != typeid(RenderType)) {
      LOG::Fatal(__FILE__, __LINE__) << "Third argument must be RenderType";
      abort();
    } else {
      render_type_ = std::any_cast<RenderType>(args[2]);
      if (render_type_ == RenderType::NONE) {
        LOG::Fatal(__FILE__, __LINE__) << "Render type NONE is invalid here";
        abort();
      }
    }
    switch (render_type_) {
      case RenderType::RENDER_2D:
        render_2d_ = new Render2D();
        render_2d_->Set_WindowRef(window_);
        render_2d_->Set_EntityManager(entity_manager_);
        // TODO ^ перевести их в Init()
        render_2d_->Init();
        break;
      case RenderType::RENDER_3D:
        render_3d_ = new Render3D();
        break;
      default:
        LOG::Fatal(__FILE__, __LINE__) << "Unknown render type";
        abort();
        break;
    }
  }

  void FreeResources() override {
    switch (render_type_) {
      case RenderType::RENDER_2D:
        if (render_2d_) {
          delete render_2d_;
          render_2d_ = nullptr;
        }
        break;
      case RenderType::RENDER_3D:
        if (render_3d_) {
          delete render_3d_;
          render_3d_ = nullptr;
        }
        break;
      default:
        LOG::Fatal(__FILE__, __LINE__) << "Unknown render type";
        abort();
        break;
    }
    LOG::Debug() << "Render manager resources freed.";
  }

 private:
  void Set_WindowRef(sf::RenderWindow* window = nullptr) {
    if (window)
      window_ = window;
    else
      LOG::Fatal(__FILE__, __LINE__) << "Window pointer is null";
    abort();
  }

  void Set_EntityManager(Entity* entity_manager = nullptr) {
    if (entity_manager)
      entity_manager_ = entity_manager;
    else
      LOG::Fatal(__FILE__, __LINE__) << "Entity manager pointer is null";
    abort();
  }
};

}  // namespace EDD::Managers
