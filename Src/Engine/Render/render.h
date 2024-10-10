#pragma once

#include <GLFW/glfw3.h>
#include <algorithm>
#include <vector>
#include "../GameEntity/EngineEntity/engine_entity.h"

class Render {
 private:
 public:
  // Метод для добавления нового наблюдателя
  void Attach(GameEntity* observer);

  // Метод для удаления наблюдателя
  void Detach(GameEntity* observer);

  // Уведомляет наблюдателей о будующем обновлении рендера
  void UpdateGameEntity(GameEntity* observer);

  // Обновдение рендера
  void UpdateRender();
};

void Render::UpdateGameEntity(GameEntity* observer) {

}

void Render::UpdateRender() {
  // Очищаем экран
  glClear(GL_COLOR_BUFFER_BIT);
  // Обновляем экран
  // glfwSwapBuffers();
}