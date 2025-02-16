// Managers/manager_resourse.h
#pragma once

#include "manager_base.h"
/**
 * @brief Менеджер отвечающий за обработку ресурсов (текстуры , аудио, локальные
 * файлы, ...)
 *
 */
class ManagerResourse : public ManagerBase {
 public:
  ManagerResourse() {}
  ~ManagerResourse() {}

 public:
  virtual void Update() override {}

 private:
  virtual void Init() override {}
  virtual void FreeResources() override {}
};