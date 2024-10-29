#pragma once
#include <cassert>
#include <vector>

#include "GameEntity/game_entity.h"
#include "GameLoop/game_loop.h"

#include "Inputs/KeyboardInputs/keyboard_input.h"
#include "Inputs/MouseInputs/mouse_inputs.h"
#include "Render/render.h"
#include "VieportManager/vieport_manager.h"

class Engine {
 private:
  GameLoop* game_loop_;              //< цикл игрового движка
  Render* render_;                   //< объект рендера
  VieportManager* vieport_manager_;  //< менеджер вьюпортов

  KeyboardInput* keyboard_inp_;  //< объект ввода с клавиатуры
  MouseInpit* mouse_inp_;        //< объект ввода с мыши

  //
  std::vector<GameEntity*> game_entities_;  //< Список игровых сущностей
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
    if (game_loop_) {
      delete game_loop_;
    }
    if (render_) {
      delete render_;
    }
    if (mouse_inp_) {
      delete mouse_inp_;
    }
    if (keyboard_inp_) {
      delete keyboard_inp_;
    }
    if (vieport_manager_) {
      delete vieport_manager_;
    }
  }

  /**
   * \brief Инициализация движка (подключение субклассов)
   */
  void Init() {
    assert(GetEngineSettings());
    // TODO ^ сделать получение объектами настроек

    game_loop_ = new GameLoop();
    render_ = new Render();
    vieport_manager_ = new VieportManager();

    mouse_inp_ = new MouseInpit();
    keyboard_inp_ = new KeyboardInput();
  }
  /**
   * \brief Запуск игрового цикла и запуск движка
   */
  void Start() {
    vieport_manager_->CreateVieport({800, 600, "Vieport"});  // DEBUG
    //^получить переменную из GetEngineSettings и передать её в CreateVieport

    game_loop_->Start();
    game_loop_->Loop(game_entities_,    //
                     render_,           //
                     vieport_manager_,  //
                     keyboard_inp_,     //
                     mouse_inp_);
  }
  /**
   * \brief Остановка цикла и движка
   */
  void Stop() {
    game_loop_->Stop();
    assert(SetEngineSettings());
    // TODO ^ сделать заппись объектами настроек в файл
  }
};
