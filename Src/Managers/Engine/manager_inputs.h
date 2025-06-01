#pragma once
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <optional>

#include "../Tools/interface.h"
#include "manager_base.h"

namespace EDD::Managers {
class Inputs : public Base, public Tools::Interface<sf::Event> {
 private:
  sf::Event event_;
  bool has_event_ = false;
  sf::RenderWindow* window_ = nullptr;  

 public:
  // Метод для установки указателя на окно
  void SetWindow(sf::RenderWindow* window) {
    window_ = window;
  }

  // Реализация интерфейса (исправлен тип возвращаемого значения)
  virtual std::optional<sf::Event> Send() const override {
    if (has_event_) {
      return event_;
    }
    return std::nullopt;
  }

  virtual void Update() override {
    has_event_ = false;
    // Обработка всех событий в очереди
    if (window_) {
      has_event_ = window_->pollEvent(event_);
    }
  }

 private:
  virtual void Init() override {
    // Инициализация обработчика ввода
  }

  virtual void FreeResources() override {
    // Освобождение ресурсов
    window_ = nullptr;
  }
};
}  // namespace EDD::Managers