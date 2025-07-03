// Managers/manager.h
#pragma once
#include "Engine/delegate.h"
#include "Engine/interface.h"
#include "EngineError/engine_logging.h"

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
   * @brief Инициализация менеджера (При старте), инициализация не должна вызываться в конструкторе 
   */
  virtual void Init() = 0;

  /**
   * @brief Освобождение ресурсов вызывается при завершении работы менеджера или в деструкторе
   */
  virtual void FreeResources() = 0;

  virtual ~Base() {};
};
}  // namespace EDD::Managers