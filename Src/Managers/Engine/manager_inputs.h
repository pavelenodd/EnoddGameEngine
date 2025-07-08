#pragma once
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <optional>

#include "Engine/interface.h"
#include "manager_base.h"

namespace EDD::Managers {
/*
  Менеджер ввода отвечает за обработку событий ввода от пользователя.
  все события, которые он принимает передаётся в интерфейс Tools::Interface<sf::Event>.
  */
class Inputs : public Base, public Tools::Interface<sf::Event> {
 private:
  std::optional<sf::Event> event_;
  bool has_event_ = false;
  sf::RenderWindow* window_ = nullptr;  

 public:
  Inputs(sf::RenderWindow* window = nullptr)
      : window_(window) {
    Init();
  }
  ~Inputs() {
    FreeResources();
  }

  /**
   * @brief Установить окно для обработки событий
   * @param window Указатель на окно SFML
   */
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
      event_ = window_->pollEvent();
      has_event_ = event_.has_value();
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