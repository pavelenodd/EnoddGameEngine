#pragma once
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <optional>

#include "Engine/interface.h"
#include "engine_logging.h"
#include "manager_base.h"

namespace EDD::Managers {
/*
  Менеджер ввода отвечает за обработку событий ввода от пользователя.
  все события, которые он принимает передаётся в интерфейс Tools::Interface<sf::Event>.
  */
class Inputs : public Base, public Tools::Interface<sf::Event>, public Tools::Delegate<sf::Event> {
 private:
  std::optional<sf::Event> event_;
  bool has_event_ = false;
  sf::RenderWindow* window_ = nullptr;

  // Подписчики на события интерфейса
  std::vector<Tools::Interface<sf::Event>*> observers_;

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
  void SetWindow(sf::RenderWindow* window = nullptr) {
    if (window) {
      window_ = window;
    } else {
      LOG::Fatal("window is null");
    }
  }

  virtual void Update() override {
    has_event_ = false;
    if (window_) {
      event_ = window_->pollEvent();
      has_event_ = event_.has_value();
      if (has_event_) {
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
    if (has_event_) {
      return event_;
    }
    return std::nullopt;
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