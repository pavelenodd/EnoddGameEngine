#pragma once
#include <functional>
#include <vector>

namespace EDD::Tools {
template <typename... Args>
class Delegate {
 private:
  using Callback = std::function<void(Args...)>;
  std::vector<Callback> listeners_;

 public:
  // Подписать слушателя
  void AddListener(const Callback& listener) {
    listeners_.emplace_back(listener);
  }

  // Вызвать всех подписчиков с переданными аргументами
  void Broadcast(Args... args) const {
    for (const auto& listener : listeners_) {
      listener(args...);
    }
  }

  // Очистить всех слушателей
  void Clear() { listeners_.clear(); }
};
}  // namespace EDD::Tools
