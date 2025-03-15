#pragma once
#include <string>
namespace EDD::Data {

/**
 * @brief Структура данных о вьюпорте
 *
 */
struct Viewport {
  std::string viewport_name;  // имя вьюпорта
  int w;                      // ширина
  int h;                      // высота
};
}  // namespace EDD