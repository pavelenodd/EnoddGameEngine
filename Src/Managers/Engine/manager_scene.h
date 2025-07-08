#pragma once
// менеджер созданя сцены и управлении окнами
#include <SFML/Graphics.hpp>
#include <SFML/Window/Event.hpp>
#include <optional>
#include <string>

#include "../EngineData/engine_data.h"
#include "manager_base.h"
#include "manager_inputs.h"
#include "manager_render.h"

namespace EDD::Managers {

/*
  Менеджер сцены отвечает за создание и управление сценами(вьюпортами) в игре.
      принимает настройки вьюпорта (размеры окна, имя вьюпорта)
      принимает интерфейс для обработки событий
  */
class Scene : public Managers::Base {
 private:
  sf::RenderWindow* window_ = nullptr;                 // Указатель на окно
  Data::Viewport viewport_data_;                       // Данные о вьюпорте
  bool* is_gameloop_enabled_;                          // Указатель на флаг активности игрового цикла
  const Tools::Interface<sf::Event>* event_provider_;  // Обработчик событий
  Inputs* input_manager_ = nullptr;                    // Указатель на менеджер ввода
  Render* render_manager_ = nullptr;                   // Указатель на менеджер рендера
  Entity* entity_manager_ = nullptr;                   // Указатель на менеджер сущностей

 public:
  //!!! TODO изменить обработку событий с ManagerInputs на Tools::Interface<sf::Event>
  /**
   * @brief Construct a new Scene object
   *
   * @param viewport_data // Настройки окна
   * @param event_provider // обработчик событий
   * @param is_gameloop_enabled // указатель на флаг, игрового цикла
   */
  Scene(Data::Viewport viewport_data, const Tools::Interface<sf::Event>* event_provider, bool* is_gameloop_enabled)
      : viewport_data_(viewport_data)
      , event_provider_(event_provider)
      , is_gameloop_enabled_(is_gameloop_enabled) {
    input_manager_ = const_cast<Inputs*>(dynamic_cast<const Inputs*>(event_provider));
    LOG::Debug("ManagerScene created");
    if (!input_manager_) {
      LOG::Fatal("Failed to cast event_provider to Inputs");
      return;
    }
  }

  ~Scene() {
    FreeResources();
  }

  // Удаление конструкторов копирования и перемещения
  Scene() = delete;                         // Запрет на создание без параметров
  Scene(const Scene&) = delete;             // Запрет на копирование
  Scene(Scene&&) = delete;                  // Запрет на перемещение
  Scene& operator=(const Scene&) = delete;  // Запрет на присваивание
  Scene& operator=(Scene&&) = delete;       // Запрет на перемещение присваивания

  /**
   * @brief Update the scene
   *
   */
  void Update() override {
    if (!window_ || !window_->isOpen()) { return; }

    // ! INFO Обработка событий ввода
    if (event_provider_->Send().has_value()) {
      auto event = event_provider_->Send().value();
      if (auto closedEvent = event.getIf<sf::Event::Closed>()) {
        if (window_) { window_->close(); }
      }
    }

    window_->clear(sf::Color::Black);

    // Рендеринг объектов через менеджер рендера
    if (render_manager_) { render_manager_->Update(); }

    window_->display();
  }

  /**
   * @brief Установка менеджера сущностей
   */
  void SetEntityManager(Entity* entity_manager) {
    // entity_manager_ = entity_manager;
    // if (render_manager_) { render_manager_->SetEntityManager(entity_manager_); }
  }

  /**
   * @brief Установка менеджера рендера
   */
  void SetRenderManager(Render* render_manager) {
    // render_manager_ = render_manager;
    // if (render_manager_ && window_) { render_manager_->SetWindow(window_); }
    // if (render_manager_ && entity_manager_) { render_manager_->SetEntityManager(entity_manager_); }
  }

  sf::RenderWindow* GetWindow() const {
    return window_;
  }

 private:
  void Init() override {
    LOG::Debug("ManagerScene call init");
    if (CreateScene()) {
      if (render_manager_) {
        render_manager_->SetWindow(window_);
        // if (entity_manager_) { render_manager_->SetEntityManager(entity_manager_); }
      }
    } else {
      LOG::Fatal("Failed to create scene or set input manager");
      return;
    }
  }

  void FreeResources() override {
    LOG::Debug("ManagerScene call destroy");
    if (window_) {
      window_->close();
      delete window_;
      window_ = nullptr;
    }
  }

  bool CreateScene() {
    window_ = new sf::RenderWindow(sf::VideoMode(sf::Vector2u(viewport_data_.w, viewport_data_.h)),
                                   viewport_data_.viewport_name, sf::Style::Default);

    if (!window_ || !window_->isOpen()) { return false; }
    window_->setFramerateLimit(60);
    return true;
  }
};

}  // namespace EDD::Managers