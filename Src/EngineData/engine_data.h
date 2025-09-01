#pragma once
#include <GLFW/glfw3.h>

#include <string>

namespace EDD::Data {
/**
 * @brief Структура данных о вьюпорте
 * @param viewport_name имя вьюпорта
 * @param w ширина
 * @param h высота
 */
struct Viewport {
  GLFWwindow* viewport_window = nullptr;  // окно вьюпорта
  std::string name = "";      // имя вьюпорта
  int w = 800;                // ширина
  int h = 600;                // высота
};

}  // namespace EDD::Data
