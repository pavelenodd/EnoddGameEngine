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
  GameLoop() {

  }
  ~GameLoop() {}

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
            KeyboardInpit* L_keyboard_inp,
            MouseInpit* L_mouse_inp);
};

void GameLoop::Start() {
  is_running_ = true;
}

void GameLoop::Stop() {
  is_running_ = false;
}

void GameLoop::Loop(std::vector<GameEntity*> L_entities,
                    Render* L_render,
                    VieportManager* L_vieport_manager,
                    KeyboardInpit* L_keyboard_inp,
                    MouseInpit* L_mouse_inp) {
  static KeyboardInpit* static_keyboard_inp_ = L_keyboard_inp;
  static MouseInpit* static_mouse_inp_ = L_mouse_inp;

  // Обработка событий ввода клавиатуры
  glfwSetKeyCallback(
      L_vieport_manager->Get_CurrWindow(),
      [](GLFWwindow* window, int key, int scancode, int action, int mods) {
        static_keyboard_inp_->HandlingInputs(window, key, scancode, action,
                                             mods);
      });

  // Обработка событий ввода мыши
  glfwSetKeyCallback(
      L_vieport_manager->Get_CurrWindow(),
      [](GLFWwindow* window, int key, int scancode, int action, int mods) {
        static_mouse_inp_->HandlingInputs(window, key, scancode, action, mods);
      });

  while (is_running_) {
    glfwPollEvents();

    // Обновление рендера
    L_render->UpdateRender();
    if (!L_entities.empty()) {
      for (auto entity : L_entities) {
        entity->UpdateEvent();  // Обновляем логику сущности
        L_render->UpdateGameEntity(
            entity);  // Уведоляем сущность об обновлении рендера
      }
      // Обновляем рендер второй раз после обновления сущностей
      L_render->UpdateRender();
    }
    // Обновляем вьюпорт
    L_vieport_manager->UpdateVieport();

    if (glfwWindowShouldClose(L_vieport_manager->Get_CurrWindow())) {
      is_running_ = false;
    }
  }
  is_running_ = false;
}