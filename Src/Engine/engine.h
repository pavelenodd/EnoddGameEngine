#pragma once
#include <cassert>
#include <vector>

#include "EngineEntity/Window/vieport_manager.h"
#include "GameEntity/game_entity.h"
#include "GameLoop/game_loop.h"
#include "Inputs/keyboard_input.h"
#include "Inputs/mouse_inputs.h"
#include "Render/render.h"

class Engine {
 private:
  GameLoop* game_loop_;   // цикл игрового движка
  Render* render_;        // объект рендера
  Inputs* keyboard_inp_;  // объект ввода с клавиатуры
  Inputs* mouse_inp_;     // объект ввода с мыши

                          //
  std::vector<GameEntity*> game_entities_;  // Список игровых сущностей
 public:
 private:
  /**
   * \brief Загрузка настроек движка
   * \return true, если все настройки были загружены, false если какая-то
   * настройка не была загружена
   */
  bool GetEngineSettings() {
    bool flag = true;
    /*
    здесь получаются настройки движка
    если какой то параметр не задан, то возвращается flag == false
    */

    return flag;
  }
  /**
   * \brief Сохранение настроек движка
   * \return true, если все настройки были сохранены, false если какая-то
   * настройка не была сохранена
   */
  bool SetEngineSettings() {
    bool flag = true;
    /*
    здесь сохраняются настройки движка
    если какой то параметр неуспешно запишется, то возвращается flag == false
    */
    return flag;
  }

 public:
  ~Engine() {
    delete game_loop_;
    delete render_;
  }

  /**
   * \brief Инициализация движка (подключение субклассов)
   */
  void Init() {
    assert(GetEngineSettings());

    game_loop_ = new GameLoop();
    render_ = new Render();
    mouse_inp_ = new MouseInpit();
    keyboard_inp_ = new KeyboardInpit();
    }
  /**
   * \brief Запуск игрового цикла и запуск движка
   */
  void Start() {
    game_loop_->Start();
    game_loop_->Loop(game_entities_, render_);
  }
  /**
   * \brief Остановка цикла и движка
   */
  void Stop() {
    game_loop_->Stop();
    assert(SetEngineSettings());
  }
};
