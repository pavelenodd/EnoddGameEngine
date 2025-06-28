#pragma once
#include <string>
namespace EDD::Data {
/**
 * @brief Структура данных о вьюпорте
 * @param viewport_name имя вьюпорта
 * @param w ширина
 * @param h высота
 */
struct Viewport {
  std::string viewport_name;  // имя вьюпорта
  int w;                      // ширина
  int h;                      // высота
};

}  // namespace EDD::Data
