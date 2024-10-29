#pragma once
#include <vector>
#include "../GameEntity/EngineEntity/engine_entity.h"
#include "../GameEntity/game_entity.h"

#include "../Inputs/KeyboardInputs/keyboard_input.h"
#include "../Inputs/MouseInputs/mouse_inputs.h"

#include "../Render/render.h"
#include "../VieportManager/vieport_manager.h"

class GameLoop {
 private:
  bool is_running_ = false;

 public:
  GameLoop() {}
  ~GameLoop() {}
  GameLoop(const GameLoop&) = delete;
  GameLoop& operator=(const GameLoop&) = delete;
  GameLoop(GameLoop&&) = delete;
  GameLoop& operator=(GameLoop&&) = delete;
  /**
   * \brief Обработка событий ввода
   */
  static void KeyCallback(GLFWwindow* window,
                          int key,
                          int scancode,
                          int action,
                          int mods,
                          Inputs* L_inputs) {
    if (L_inputs) {
      // Вызов обработчика ввода через указатель на виртуальный класс
      L_inputs->HandlingInputs(window, key, scancode, action, mods);
    }
  }
  /**
   * \brief Запуск цикла игрового движка
   */
  void Start();

  /**
   * \brief Остановка цикла игрового движка
   */
  void Stop();
  /**
   * \brief Цикл игрового движка
   * \param L_entities Список сущностей для обновления
   * \param L_render Объект рендера
   * \param L_vieport_manager Объект менеджера вьюпорта
   * \param L_keyboard_inp Объект ввода с клавиатуры
   * \param L_mouse_inp Объект ввода с мыши
   */
  void Loop(std::vector<GameEntity*> L_entities,
            Render* L_render,
            VieportManager* L_vieport_manager,
            KeyboardInput* L_keyboard_inp,
            MouseInpit* L_mouse_inp);
};

void GameLoop::Start() {
  is_running_ = true;
}

void GameLoop::Stop() {
  is_running_ = false;
}

// Обработка событий ввода клавиатуры

void GameLoop::Loop(std::vector<GameEntity*> L_entities,
                    Render* L_render,
                    VieportManager* L_vieport_manager,
                    KeyboardInput* L_keyboard_inp,
                    MouseInpit* L_mouse_inp) {
  //
  static KeyboardInput* static_keyboard_inp_ = L_keyboard_inp;
  static MouseInpit* static_mouse_inp_ = L_mouse_inp;
  // INFO ^ объекты статичные тк Callback не принимает локальные переменные

  // Обработка событий ввода клавиатуры
  glfwSetKeyCallback(
      L_vieport_manager->Get_CurrWindow(),
      [](GLFWwindow* window, int key, int scancode, int action, int mods) {
        KeyCallback(window, key, scancode, action, mods, static_keyboard_inp_);
      });

  // Обработка событий ввода мыши
  glfwSetMouseButtonCallback(
      L_vieport_manager->Get_CurrWindow(),
      [](GLFWwindow* window, int button, int action, int mods) {
        KeyCallback(window, button, -1, action, mods, static_mouse_inp_);
      });

  while (is_running_) {
    if (glfwWindowShouldClose(L_vieport_manager->Get_CurrWindow())) {
      is_running_ = false;
    }
    glfwPollEvents();

    // Обновление рендера
    L_render->UpdateRender();
    if (!L_entities.empty()) {
      // TODO вывести в отдельный поток или сделать асинхронный вызов
      for (auto entity : L_entities) {
        entity->UpdateEvent();  // Обновляем логику сущности
        L_render->UpdateGameEntity(
            entity);  // Уведоляем сущность об обновлении рендера
      }
      // Обновляем рендер второй раз после обновления сущностей
      L_render->UpdateRender();
    }
    glfwPollEvents();
    // Обновляем вьюпорт
    L_vieport_manager->UpdateVieport();
  }
  is_running_ = false;
}