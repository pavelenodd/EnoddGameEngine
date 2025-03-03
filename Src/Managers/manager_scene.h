// Managers/manager_scene.h

#pragma once

#include "manager_base.h"
/**
 * @brief Менеджер отвечающий за создание и обновление сцены
 *
 */
class ManagerScene : public ManagerBase {
 private:
 public:
  ManagerScene() {}
  ~ManagerScene() {}

 public:
  /**
   * @brief Обновление Вьюпорта
   *
   */
  virtual void Update() override {}

 private:
  virtual void Init() override {
    // Инициализация переменных ......

    CreateScene();
  }
  virtual void FreeResources() override {}
  void CreateScene() {}
};