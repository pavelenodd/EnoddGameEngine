#pragma once
// менеджер созданя сцены и управлении окнами
#include <SFML/Graphics.hpp>
#include <SFML/Window/Event.hpp>
#include <SFML/Window/VideoMode.hpp>
#include <string>

#include "../EngineData/engine_data.h"
#include "manager_base.h"

namespace EDD::Managers {

/*
  Менеджер сцены отвечает за создание и управление сценами(вьюпортами) в игре.
      принимает настройки вьюпорта (размеры окна, имя вьюпорта)
      принимает интерфейс для обработки событий
      принимает указатель на менеджер
  */
using InterfaceSFEvent = Tools::Interface<sf::Event>;
class Scene : public Managers::Base {
 private:
  sf::RenderWindow *window_ = nullptr;                 // Указатель на окно
  Data::Viewport viewport_data_;                       // Данные о вьюпорте
  bool *is_gameloop_enabled_;  // Указатель на флаг активности игрового цикла

 public:
  /**
   * @brief Construct a new Scene object
   *
   * @param viewport_data // Настройки окна
   * @param is_gameloop_enabled // указатель на флаг, игрового цикла
   */
  Scene(Data::Viewport viewport_data, bool *is_gameloop_enabled)
      : viewport_data_(viewport_data), is_gameloop_enabled_(is_gameloop_enabled) {
    LOG::Debug("Manager Scene created");
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
  void Update() override {}

  sf::RenderWindow *GetWindowRef() const {
    return window_;
  }

 private:
  void Init() override {
    // Создаем окно с заданными параметрами
    window_ = new sf::RenderWindow(
        sf::VideoMode(sf::Vector2u(viewport_data_.w, viewport_data_.h)),
        viewport_data_.name,
        sf::Style::Default,
        sf::State::Windowed,
        sf::ContextSettings(24, 8, 4, 3, 3));

    if (!window_) {
      LOG::Fatal("Failed to create window");
      return;
    }

    LOG::Info("Window created with size: " + std::to_string(viewport_data_.w) + "x" +
              std::to_string(viewport_data_.h));
  }
  void FreeResources() override {
    LOG::Debug("ManagerScene call destroy");
  }

  bool CreateScene() {
    return true;
  }
};

}  // namespace EDD::Managers