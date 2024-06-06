#pragma once

#include "../GameObjects/game_objects.h"
namespace edd {

class Render : protected GameObject {
 private:
  /* data */
 public:
  /**
   * @brief Интерфейс рендера объектов
   */
  void DrawObjects(std::vector<GameObject*> objects) {
    for (auto obj : objects) {
      obj->Draw();
    }
  };
};

}  // namespace edd