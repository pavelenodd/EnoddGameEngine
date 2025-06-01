#pragma once
#include <optional>

namespace EDD ::Tools {

// Интерфейс предоставляет данные любого типа
template <typename T>
class Interface {
 public:
  virtual std::optional<T> Send() const = 0;

  virtual ~Interface() = default;
};

}  // namespace EDD::Tools