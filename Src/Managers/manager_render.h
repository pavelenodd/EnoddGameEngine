#pragma once
//
// Менеджер рендера (3D + 2D) на bgfx, окно — SFML

#include <bgfx/bgfx.h>
#include <bgfx/platform.h>
#include <bx/math.h>

#include <SFML/Graphics.hpp>

#include "Items/Components/comp_render2D.h"
#include "Items/Components/comp_render3D.h"
#include "manager_base.h"
#include "manager_entity.h"
#include "manager_resource.h"

namespace EDD::Managers {

class Render : public Managers::Base {
 private:
  sf::RenderWindow* window_;
  Render2D* render_2d_;
  // Render3D* render_3d_;
  Managers::Entity* entity_manager_;
  Managers::Resource* resource_manager_;

  bool owns_render2d_ = false;  // ADD: ownership flag if we allocate
  bool owns_render3d_ = false;  // ADD: ownership flag if we allocate
 public:
  Render() {}
  ~Render() {
    FreeResources();
  }

  void Update() override {
    if (!window_ || !entity_manager_) return;
    if (render_2d_) {
      render_2d_->Render_All(*entity_manager_);
    }

    bgfx::frame();
  }

  void Init(std::vector<std::any> args) override {
    LOG::Debug() << "Render manager initialized.";

    if (!render_2d_) {
      render_2d_ = new Render2D();
      owns_render2d_ = true;
    }

    if (!window_) {
      LOG::Fatal(__FILE__, __LINE__) << "Render: window pointer is null";
    }
    if (!entity_manager_) {
      LOG::Fatal(__FILE__, __LINE__) << "Render: entity manager pointer is null";
    }

    render_2d_->Init();
    render_2d_->Set_WindowRef(window_);
    render_2d_->Set_EntityManager(entity_manager_);
  }

  void FreeResources() override {
    if (owns_render2d_ && render_2d_) {
      delete render_2d_;
      render_2d_ = nullptr;
      owns_render2d_ = false;
    }
    LOG::Debug() << "Render manager resources freed.";
  }

 private:
  void Set_WindowRef(sf::RenderWindow* window = nullptr) {
    if (window)
      window_ = window;
    else
      LOG::Fatal(__FILE__, __LINE__) << "Window pointer is null";
  }

  void Set_EntityManager(Entity* entity_manager = nullptr) {
    if (entity_manager)
      entity_manager_ = entity_manager;
    else
      LOG::Fatal(__FILE__, __LINE__) << "Entity manager pointer is null";
  }
};

}  // namespace EDD::Managers
