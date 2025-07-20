#pragma once
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <SFML/Window/Keyboard.hpp>
#include <algorithm>
#include <optional>
#include <unordered_set>

#include "EngineError/engine_logging.h"
#include "Tools/Engine/delegate.h"
#include "Tools/Engine/interface.h"
#include "algorithm"
#include "manager_base.h"

namespace EDD::Managers {
using InterfaceSFEvent = Tools::Interface<sf::Event>;
/*
  Менеджер ввода отвечает за обработку событий ввода от пользователя.
  все события, которые он принимает передаётся в интерфейс Tools::Interface<sf::Event>.
  */
class Inputs : public Base, public InterfaceSFEvent, public Tools::Delegate<sf::Event> {
 private:
  std::optional<sf::Event> event_ = std::nullopt;
  sf::RenderWindow* window_ = nullptr;

  // Подписчики на события интерфейса
  std::unordered_set<InterfaceSFEvent*> observers_;

 public:
  Inputs() {}
  ~Inputs() {
    FreeResources();
  }

  virtual void Update() override {
    LOG::Info("Input manager updated.");
    // if (window_) {
    //   event_ = window_->pollEvent();
    //   if (event_.has_value() && event_->is<sf::Event::KeyPressed>()) {
    //     // Уведомляем всех подписчиков
    //     for (auto* obs : observers_) {
    //       obs->interface_args_ = event_;
    //     }
    //   }
    // }
  }
  // Подписаться/отписаться на события
  void Subscribe(InterfaceSFEvent* observers = nullptr) {
    if (observers != nullptr) {
      observers_.insert(observers);
    }
  }
  void Unsubscribe(InterfaceSFEvent* observers = nullptr) {
    if (observers != nullptr) {
      observers_.erase(observers);
    }
  }

 private:
  virtual void Init(std::initializer_list<void*> args = {}) override {
    // Инициализация обработчика ввода
    LOG::Info("Input manager initialized.");
    if (args.size() == 0) {
      LOG::Fatal("No window provided for Inputs manager initialization");
    }
    if (args.begin() == args.end() || *args.begin() == nullptr) {
      LOG::Fatal("Window pointer is null in Inputs manager initialization");
    }
    if (static_cast<sf::RenderWindow*>(*args.begin()) == nullptr) {
      LOG::Fatal("Window pointer is null in Inputs manager initialization");
    }
    std::for_each(args.begin(), args.end(), [this](const auto& arg) {
      SetWindowRef(static_cast<sf::RenderWindow*>(arg));
    });
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
  // INFO: для тестов
  friend class TestManagerInputs;
};
}  // namespace EDD::Managers