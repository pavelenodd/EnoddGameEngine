#pragma once
#include <GLFW/glfw3.h>
#include "window.h"
/**
 * \brief Объект вьюпорта
 */
class GameVieport : public Window {
 private:
  GLFWwindow* window_;

 public:
  GameVieport(const WindowSettings& L_winow_settings) {
    this->Create(L_winow_settings);
  }
  GameVieport(const WindowSettings&& L_winow_settings) {
    this->Create(L_winow_settings);
  }
  virtual ~GameVieport() {
    // Освобождаем ресурсы
    glfwDestroyWindow(this->window_);
    glfwTerminate();
    delete this->window_;
  }
  /**
   * \brief Обновление вьюпорта
   */
  virtual void Update() {}

 private:
  /** \brief Cоздание нового активного вьюпорта
   *
   */
  virtual void Create(const WindowSettings& L_winow_settings) {
    // Создаем окно
    GLFWwindow* window =
        glfwCreateWindow(L_winow_settings.height, L_winow_settings.width,
                         L_winow_settings.window_name_.c_str(), NULL, NULL);
    if (!window_) {
      glfwTerminate();
      return;
    }

    // Делаем контекст текущим
    glfwMakeContextCurrent(window_);
  }
};