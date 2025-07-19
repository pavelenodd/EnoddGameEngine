#pragma once
#include <list>
#include <optional>
namespace EDD ::Tools {

// Интерфейс предоставляет данные любого типа
template <typename T>
class Interface {
 public:
  std::list<std::optional<T>> interface_args_;  // Список передаваемых значений

  virtual ~Interface() = default;
};

}  // namespace EDD::Tools