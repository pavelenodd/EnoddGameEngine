#pragma once
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <optional>

#include "EngineError/engine_logging.h"
#include "Tools/Engine/delegate.h"
#include "Tools/Engine/interface.h"
#include "manager_base.h"

namespace EDD::Managers {
/*
  Менеджер ввода отвечает за обработку событий ввода от пользователя.
  все события, которые он принимает передаётся в интерфейс Tools::Interface<sf::Event>.
  */
class Inputs : public Base, public Tools::Interface<sf::Event>, public Tools::Delegate<sf::Event> {
 private:
  std::optional<sf::Event> event_;
  sf::RenderWindow* window_ = nullptr;

  // Подписчики на события интерфейса
  std::vector<Tools::Interface<sf::Event>*> observers_;

 public:
  Inputs() {}
  ~Inputs() {
    FreeResources();
  }

  virtual void Update() override {
    LOG::Info("Input manager updated.");
    if (window_) {
      event_ = window_->pollEvent();
      if (event_.has_value()) {
        // Уведомляем всех подписчиков
        for (auto* obs : observers_) {
          if (obs) obs->Send();
        }
      }
    }
  }
  // Подписаться/отписаться
  void Subscribe(Tools::Interface<sf::Event>* observers) {
    if (observers) {
      observers_.push_back(observers);
    }
  }
  void Unsubscribe(Tools::Interface<sf::Event>* observers) {
    observers_.erase(std::remove(observers_.begin(), observers_.end(), observers),
                     observers_.end());
  }
  // Реализация отправки события
  virtual std::optional<sf::Event> Send() const override {
    if (event_.has_value()) {
      return event_;
    }
    return std::nullopt;
  }

 private:
  virtual void Init(std::initializer_list<void*> args = {}) override {
    // Инициализация обработчика ввода
    LOG::Info("Input manager initialized.");
    SetWindowRef(static_cast<sf::RenderWindow*>(*args.begin()));
  }

  virtual void FreeResources() override {
    // Освобождение ресурсов
    window_ = nullptr;
  }
  /**
   * @brief Установить окно для обработки событий
   * @param window Указатель на окно SFML
   */
  void SetWindowRef(sf::RenderWindow* window = nullptr) {
    if (window) {
      window_ = window;
    } else {
      LOG::Fatal("window is null");
    }
  }
};
}  // namespace EDD::Managers