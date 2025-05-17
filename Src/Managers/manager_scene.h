#pragma once
#include <SFML/Graphics.hpp>
#include <string>
#include "../EngineData/engine_data.h"
#include "manager_base.h"
#include "manager_inputs.h"

namespace EDD::Managers {

class Scene : public Managers::Base {
 private:
  sf::RenderWindow* window_ = nullptr;
  Data::Viewport viewport_data_;
  bool* is_gameloop_enabled_;
  const Tools::Interface<sf::Event>* event_provider_;
  Inputs* input_manager_ = nullptr;  // Для установки указателя на окно

 public:
  Scene(Data::Viewport viewport_data,
        const Tools::Interface<sf::Event>* event_provider,
        bool* is_gameloop_enabled)
      : viewport_data_(viewport_data),
        event_provider_(event_provider),
        is_gameloop_enabled_(is_gameloop_enabled) {
    // Преобразуем указатель для доступа к методу SetWindow
    input_manager_ = const_cast<Inputs*>(dynamic_cast<const Inputs*>(event_provider));
  }

  // Уберите ключевое слово override, если Base::~Base() не виртуальный
  ~Scene() {
    FreeResources();
  }

  void Update() override {
    // Проверяем, что окно существует
    if (!window_ || !window_->isOpen()) {
      return;
    }

    // Обработка событий ввода
    if (auto event = event_provider_->Send()) {
      if (event->type == sf::Event::Closed) {
        *is_gameloop_enabled_ = false;
      }
      if (event->type == sf::Event::KeyPressed && event->key.code == sf::Keyboard::Escape) {
        *is_gameloop_enabled_ = false;
      }
    }

    window_->clear(sf::Color::Black);
    // TODO: отрисовка объектов сцены
    window_->display();
  }

 private:
  void Init() override { 
    if (CreateScene() && input_manager_) {
      // Устанавливаем указатель на окно в менеджер ввода
      input_manager_->SetWindow(window_);
    }
  }

  void FreeResources() override {
    if (window_) {
      window_->close();
      delete window_;
      window_ = nullptr;
    }
  }

  bool CreateScene() {
    window_ = new sf::RenderWindow(
        sf::VideoMode(viewport_data_.w, viewport_data_.h),
        viewport_data_.viewport_name,
        sf::Style::Default
    );
    if (!window_ || !window_->isOpen()) {
      // Можно добавить логирование ошибки
      return false;
    }
    window_->setFramerateLimit(60);
    return true;
  }
};

} // namespace EDD::Managers