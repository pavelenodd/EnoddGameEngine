#pragma once
#include <optional>
namespace EDD ::Tools {

// Интерфейс предоставляет данные любого типа
template <typename T>
class Interface {
 public:
  std::optional<T> interface_args_;  // Передаваемое значение

  virtual ~Interface() = default;
};

}  // namespace EDD::Tools