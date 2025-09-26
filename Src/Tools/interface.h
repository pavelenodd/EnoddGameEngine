#pragma once
#include <vector>

namespace EDD::Tools {
// Интерфейс предоставляет данные любого типа
/*
Интерфейс нужен для передачи данных между модулями без выполнения
т.е. в одном месте данные передаются в интерфейс, а в другом месте эти данные обрабатываются ,
то же и с функциями
*/
template <typename T>
class Interface {
 public:
  std::vector<T> interface_args_;  // Передаваемое значения

  virtual ~Interface() = default;
};

}  // namespace EDD::Tools