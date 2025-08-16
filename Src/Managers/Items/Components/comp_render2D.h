#pragma once
//
// Компоненты для 3D рендера (bgfx)

#include <bgfx/bgfx.h>
#include <bx/math.h>

namespace EDD::Managers {
class Render2D {
 public:
  void Render() {}

 public:
  void Set_WindowRef(auto &&window) {}
  void Set_EntityManager(auto &&entity_manager) {}
  void Init() {
    // Инициализация 2D рендера
  }

  void Render_All(auto &&entities) {}
};

}  // namespace EDD::Managers
