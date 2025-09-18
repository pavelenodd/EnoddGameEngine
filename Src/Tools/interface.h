#pragma once
#include <vector>

namespace EDD::Tools {
// Интерфейс предоставляет данные любого типа
template <typename T>
class Interface {
 public:
  std::vector<T> interface_args_;  // Передаваемое значения

  virtual ~Interface() = default;
};

}  // namespace EDD::Tools