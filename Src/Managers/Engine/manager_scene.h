#pragma once
// менеджер созданя сцены и управлении окнами
#include <SFML/Graphics.hpp>
#include <SFML/Window/Event.hpp>
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
      принимает указатель на менеджер
  */
using InterfaceSFEvent = Tools::Interface<sf::Event>;
class Scene : public Managers::Base {
 private:
  sf::RenderWindow *window_ = nullptr;                 // Указатель на окно
  Data::Viewport viewport_data_;                       // Данные о вьюпорте
  bool *is_gameloop_enabled_;                          // Указатель на флаг активности игрового цикла
  const InterfaceSFEvent *event_provider_;             // Обработчик событий
  Inputs *input_manager_ = nullptr;                    // Указатель на менеджер ввода
  Render *render_manager_ = nullptr;                   // Указатель на менеджер рендера
  Entity *entity_manager_ = nullptr;                   // Указатель на менеджер сущностей

 public:
  /**
   * @brief Construct a new Scene object
   *
   * @param viewport_data // Настройки окна
   * @param event_provider // обработчик событий
   * @param is_gameloop_enabled // указатель на флаг, игрового цикла
   */
  Scene(Data::Viewport viewport_data,            // базовые настройки вьюпорта
        const InterfaceSFEvent *event_provider,  // обработчик событий
        const Managers::Render *render_manager,  // менеджер рендера
        bool *is_gameloop_enabled)
      : viewport_data_(viewport_data),
        event_provider_(event_provider),
        is_gameloop_enabled_(is_gameloop_enabled),
        render_manager_(const_cast<Managers::Render *>(render_manager)) {
    LOG::Debug("Manager Scene created");

    if (!input_manager_) {
      LOG::Fatal("Failed to cast event_provider to Inputs");
      // return;
    }
    if (!render_manager_) {
      LOG::Fatal("Failed to cast event_provider to Render");
      // return;
    }
    if (!entity_manager_) {
      LOG::Fatal("Failed to cast event_provider to Entity");
      // return;
    }
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

  /**
   * @brief Установка менеджера сущностей
   */
  void SetEntityManager(Entity *entity_manager) {

  }

  /**
   * @brief Установка менеджера рендера
   */
  void SetRenderManager(Render *render_manager) {

  }

  sf::RenderWindow *GetWindowRef() const {
    return window_;
  }

 private:
  void Init() override {

  }

  void FreeResources() override {
    LOG::Debug("ManagerScene call destroy");
  }

  bool CreateScene() {
    return true;
  }
};

}  // namespace EDD::Managers