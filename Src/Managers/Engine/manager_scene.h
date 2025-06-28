#pragma once
// менеджер созданя сцены и управлении окнами
#include <SFML/Graphics.hpp>
#include <string>

#include "../EngineData/engine_data.h"
#include "manager_base.h"
#include "manager_inputs.h"
#include "manager_render.h"

namespace EDD::Managers {

class Scene : public Managers::Base {
 private:
  sf::RenderWindow* window_ = nullptr;  // Указатель на окно
  Data::Viewport viewport_data_;        // Данные о вьюпорте
  bool* is_gameloop_enabled_;  // Указатель на флаг активности игрового цикла
  const Tools::Interface<sf::Event>* event_provider_;  // Обработчик событий
  Inputs* input_manager_ = nullptr;  // Указатель на менеджер ввода
  ManagerRender* render_manager_ = nullptr;  // Указатель на менеджер рендера

 public:
  /**
   * @brief Construct a new Scene object
   *
   * @param viewport_data // Настройки окна
   * @param event_provider // обработчик событий
   * @param is_gameloop_enabled // указатель на флаг, игрового цикла
   */
  Scene(Data::Viewport viewport_data,
        const Tools::Interface<sf::Event>* event_provider,
        bool* is_gameloop_enabled)
      : viewport_data_(viewport_data),
        event_provider_(event_provider),
        is_gameloop_enabled_(is_gameloop_enabled) {
    input_manager_ =
        const_cast<Inputs*>(dynamic_cast<const Inputs*>(event_provider));
  }

  ~Scene() { FreeResources(); }

  /**
   * @brief Update the scene
   *
   */
  void Update() override {
    if (!window_ || !window_->isOpen()) {
      return;
    }

    // INFO Обработка событий ввода
    // TODO Обработка событий ввода надо сделать через интерфейс
    if (auto event = event_provider_->Send()) {
      if (event->type == sf::Event::Closed) {
        *is_gameloop_enabled_ = false;
      }
      if (event->type == sf::Event::KeyPressed &&
          event->key.code == sf::Keyboard::Escape) {
        *is_gameloop_enabled_ = false;
      }
    }

    window_->clear(sf::Color::Black);

    // Рендеринг объектов через менеджер рендера
    if (render_manager_) {
      render_manager_->Update();
    }

    window_->display();
  }

  /**
   * @brief Установка менеджера рендера
   */
  void SetRenderManager(ManagerRender* render_manager) {
    render_manager_ = render_manager;
    if (render_manager_ && window_) {
      render_manager_->SetWindow(window_);
    }
  }

 private:
  void Init() override {
    if (CreateScene() && input_manager_) {
      input_manager_->SetWindow(window_);
      if (render_manager_) {
        render_manager_->SetWindow(window_);
      }
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
    window_ = new sf::RenderWindow(sf::VideoMode(viewport_data_.w, viewport_data_.h),
                             viewport_data_.viewport_name, sf::Style::Default);
                             
    if (!window_ || !window_->isOpen()) {
      return false;
    }
    window_->setFramerateLimit(60);
    return true;
  }
};

}  // namespace EDD::Managers