#pragma once
#include <GLFW/glfw3.h>
#include <algorithm>
#include <vector>
#include "../EngineEntity/engine_entity.h"

class Render {
 private:
 public:
  // Метод для добавления нового наблюдателя
  void Attach(GameEntity* observer);

  // Метод для удаления наблюдателя
  void Detach(GameEntity* observer);

  // Уведомляет наблюдателей об обовлении рендера
  void UpdateGameEntity(GameEntity* observer);

  // Обновдение рендера
  void UpdateRender();
};

void Render::UpdateGameEntity(GameEntity* observer) {
  observer->UpdateRender();
}

void Render::UpdateRender() {
  // Очищаем экран
  glClear(GL_COLOR_BUFFER_BIT);
  // Обновляем экран
  // glfwSwapBuffers();
}