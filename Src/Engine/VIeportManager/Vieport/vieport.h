#pragma once
#include <GLFW/glfw3.h>
#include <iostream>
#include "Window/window.h"
/**
 * \brief Объект вьюпорта
 */
class GameVieport : public Window {
 private:
  GLFWwindow* window_;

 public:
  /*Зона конструкторов ========================================= */
  GameVieport(const WindowSettings& L_winow_settings) {
    this->Create(L_winow_settings);
  }
  GameVieport(const WindowSettings&& L_winow_settings) {
    this->Create(L_winow_settings);
  }
  GameVieport(const GameVieport&) = delete;
  GameVieport& operator=(const GameVieport&) = delete;

  virtual ~GameVieport() {
    // Освобождаем ресурсы
    glfwDestroyWindow(window_);
    glfwTerminate();
    window_ = nullptr;

    /*==========================================================*/
  }
  /**
   * \brief Обновление вьюпорта
   */
  void Update() {}
  /**
   * \brief Возвращает указатель на окно
   */
  GLFWwindow* Get_Window() { return this->window_; }

 private:
  /** \brief Cоздание нового активного вьюпорта
   *
   */
  void Create(const WindowSettings& L_winow_settings);
  /**
   * \brief Метод обратного вызова при изменении размера окна
   */
  void FramebufferSizeCallback(WindowSettings L_winow_settings);
};

void GameVieport::Create(const WindowSettings& L_winow_settings) {
  // Инициализация GLFW
  if (!glfwInit()) {
    std::cerr << "Ошибка инициализации GLFW" << std::endl;
    return;
  }

  // Создаем окно
  window_ = glfwCreateWindow(L_winow_settings.height,
                             L_winow_settings.width,                 //
                             L_winow_settings.window_name_.c_str(),  //
                             NULL, NULL);

  // Проверка создания окна
  if (!window_) {
    std::cerr << "Ошибка инициализации GLFW" << std::endl;
    glfwTerminate();
    return;
  }
  // Устанавливаем OpenGL версии 4.6
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  // Делаем контекст текущим
  glfwMakeContextCurrent(window_);

  FramebufferSizeCallback(L_winow_settings);
}

void GameVieport::FramebufferSizeCallback(WindowSettings L_winow_settings) {
  // Обновление вьюпорта OpenGL при изменении размера окна
  glViewport(0, 0, L_winow_settings.width, L_winow_settings.height);
}