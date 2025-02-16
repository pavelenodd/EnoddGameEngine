// Managers/manager_inputs.h

#pragma once

#include "manager_base.h"
/**
 * @brief Менеджер отвечающий за обработку ввода
 *
 */
class ManagerInputs : public ManagerBase {
 private:
 public:
  ManagerInputs() {}
  ~ManagerInputs() {}

 public:
  virtual void Update() override {}
  virtual void Init() override {}
  virtual void FreeResources() override {}
};