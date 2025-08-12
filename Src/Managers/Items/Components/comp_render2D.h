#pragma once

#include <SFML/Graphics.hpp>

#include "Managers/manager_entity.h"
#include "Managers/manager_resource.h"

namespace EDD::Managers {
class Render2D {
 private:
  sf::RenderWindow* window_;
  Managers::Resource* resource_manager_;

 public:
  Render2D() {}
  ~Render2D() {}

  void Init(sf::RenderWindow* window, Managers::Resource* resource_manager) {
    window_ = window;
    resource_manager_ = resource_manager;
  }

  void Render_All(Managers::Entity& entity_manager) {
    // Render all 2D entities
  }
};
}  // namespace EDD::Managers
