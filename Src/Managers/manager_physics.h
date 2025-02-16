// Managers/manager_physics.h

#pragma once

#include "manager_base.h"
/**
 * @brief Менеджер отвечающий за обработку физики
 *
 */
class ManagerPhysics : public ManagerBase {
 private:
 public:
  ManagerPhysics() {}
  ~ManagerPhysics() {}

 public:
  virtual void Update() override {}

  virtual void Init() override {}
  virtual void FreeResources() override {}
};