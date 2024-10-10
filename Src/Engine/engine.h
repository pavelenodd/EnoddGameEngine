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
  GameLoop* game_loop_;   //< цикл игрового движка
  Render* render_;        //< объект рендера
  Inputs* keyboard_inp_;  //< объект ввода с клавиатуры
  Inputs* mouse_inp_;     //< объект ввода с мыши
  VieportManager* vieport_manager_;  //< менеджер вьюпортов

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
    vieport_manager_ = new VieportManager();
  }
  /**
   * \brief Запуск игрового цикла и запуск движка
   */
  void Start() {
    vieport_manager_->CreateVieport({800, 600, "Vieport"});  // DEBUG
    //^получить переменную из GetEngineSettings и передать её в CreateVieport

    game_loop_->Start();
    game_loop_->Loop(game_entities_,  //
                     render_,         //
                     vieport_manager_->Get_CurrVieport());
  }
  /**
   * \brief Остановка цикла и движка
   */
  void Stop() {
    game_loop_->Stop();
    assert(SetEngineSettings());
  }
};
