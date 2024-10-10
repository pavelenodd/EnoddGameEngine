#pragma once
#include "../GameEntity/EngineEntity/engine_entity.h"
/**
 * \brief Виртуальный метод для обработки событий ввода
 */
class Inputs {
 private:
  /* data */
 public:
  Inputs() {}
  ~Inputs() {}

  /**
   * \brief Обработка событий ввода
   */
  virtual void InputEvent(ObserverObject* object) = 0;
};