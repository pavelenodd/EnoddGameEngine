#pragma once
#include <GLFW/glfw3.h>

#include <cstdint>
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
  std::string title = "";                 // имя вьюпорта
  uint16_t w = 800;                       // ширина
  uint16_t h = 600;                       // высота
};

}  // namespace EDD::Data
