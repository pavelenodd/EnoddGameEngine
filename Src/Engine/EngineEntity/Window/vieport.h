#pragma once
#include "window.h"
/**
 * \brief Объект вьюпорта
 */
class GameVieport : public Window {
 private:
  /* data */
 public:
  GameVieport(/* args */) {
    this->Create();
    this->Update();
  }
  virtual ~GameVieport() {}
  /**
   * \brief Обновление вьюпорта
   */
  virtual void Update() {}

 private:
  /** \brief Cоздание нового активного вьюпорта
   *
   */
  virtual void Create() {}
};