#pragma once
#include <string>
namespace EDD {

/**
 * @brief Структура данных о вьюпорте
 *
 */
struct ViewportData {
  std::string viewport_name;  // имя вьюпорта
  int w;                      // ширина
  int h;                      // высота
};
}  // namespace EDD