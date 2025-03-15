// Managers/manager_resourse.h
#pragma once

#include "manager_base.h"
namespace EDD::Managers {

/**
 * @brief Менеджер отвечающий за обработку ресурсов (текстуры , аудио, локальные
 * файлы, ...)
 *
 */
class Resourse : public Managers::Base {
 public:
  Resourse() {}
  ~Resourse() {}

 public:
  virtual void Update() override {}

 private:
  virtual void Init() override {}
  virtual void FreeResources() override {}
};

}  // namespace EDD