#pragma once
// менеджер созданя сцены и управлении окнами
#include <SFML/Graphics.hpp>
#include <SFML/Window/Event.hpp>
#include <SFML/Window/VideoMode.hpp>

#include "../EngineData/engine_data.h"
#include "../EngineError/engine_logging.h"
#include "../Tools/Engine/interface.h"
#include "manager_base.h"

namespace EDD::Managers {

/*
  Менеджер сцены отвечает за создание и управление сценами(вьюпортами) в игре.
      принимает настройки вьюпорта (размеры окна, имя вьюпорта)
      принимает интерфейс для обработки событий
      принимает указатель на менеджер
  */
using InterfaceSFEvent = EDD::Tools::Interface<sf::Event>;
class Scene : public Managers::Base {
 private:
  sf::RenderWindow *window_ = nullptr;  // Указатель на окно
  Data::Viewport viewport_data_;        // Данные о вьюпорте
  bool *is_gameloop_enabled_;           // Указатель на флаг активности цикла

 public:
  /**
   * @brief Construct a new Scene object
   *
   * @param viewport_data // Настройки окна
   * @param is_gameloop_enabled // указатель на флаг, игрового цикла
   */
  Scene(Data::Viewport viewport_data, bool *is_gameloop_enabled)
      : viewport_data_(viewport_data), is_gameloop_enabled_(is_gameloop_enabled) {
    EDD::LOG::Debug("Manager Scene created");
  }

  ~Scene() {
    FreeResources();
  }

  // Удаление конструкторов копирования и перемещения
  Scene() = delete;                          // Запрет на создание без параметров
  Scene(const Scene &) = delete;             // Запрет на копирование
  Scene(Scene &&) = delete;                  // Запрет на перемещение
  Scene &operator=(const Scene &) = delete;  // Запрет на присваивание
  Scene &operator=(Scene &&) = delete;       // Запрет на перемещение присваивания

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
    // if (auto event = event_provider_->Send()) {
    //   if (event->type == sf::Event::Closed) {
    //     *is_gameloop_enabled_ = false;
    //   }
    //   if (event->type == sf::Event::KeyPressed &&
    //       event->key.code == sf::Keyboard::Escape) {
    //     *is_gameloop_enabled_ = false;
    //   }
    // }

    window_->clear(sf::Color::Black);

    window_->display();
  }

  sf::RenderWindow *GetWindowRef() const {
    return window_;
  }

 private:
  void Init(std::initializer_list<void *> args = {}) override {
    // Создаем окно с заданными параметрами

    CreateScene();
  }
  void FreeResources() override {
    EDD::LOG::Debug("ManagerScene call destroy");
  }

  bool CreateScene() {
    window_ = new sf::RenderWindow(
        sf::VideoMode(sf::Vector2u(viewport_data_.w, viewport_data_.h)),
        viewport_data_.name,
        sf::Style::Default,
        sf::State::Windowed,
        sf::ContextSettings(24, 8, 4, 3, 3));

    if (!window_) {
      EDD::LOG::Fatal("Failed to create window");
      return false;
    }

    // Устанавливаем размер окна
    window_->setSize(sf::Vector2u(viewport_data_.w, viewport_data_.h));
    window_->setVerticalSyncEnabled(true);  // Включаем вертикальную синхронизацию

    EDD::LOG::Info("Window created with size: " + std::to_string(viewport_data_.w) + "x" +
                   std::to_string(viewport_data_.h));
    return true;
  }
};

}  // namespace EDD::Managers