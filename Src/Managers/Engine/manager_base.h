// Managers/manager.h
#pragma once
#include "Engine/delegate.h"
#include "Engine/interface.h"
namespace EDD::Managers {
/**
 * @brief Базовый класс менеджеров
 *
 */
class Base {
 private:
 public:
  /**
   * @brief Обновление менеджера (в тике)
   *
   */
  virtual void Update() = 0;

  /**
   * @brief Инициализация менеджера (При старте)
   */
  virtual void Init() = 0;

  /**
   * @brief Освобождение ресурсов
   */
  virtual void FreeResources() = 0;
};
}  // namespace EDD::Managers