// Managers/manager.h
#pragma once
#include <initializer_list>

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
   * @brief Инициализация менеджера (При старте), инициализация не должна вызываться в
   * конструкторе
   * @param args Список указателей на контекст инициализации
   */
  virtual void Init(std::initializer_list<void*> args = {}) = 0;

  /**
   * @brief Освобождение ресурсов вызывается при завершении работы менеджера или в деструкторе
   */
  virtual void FreeResources() = 0;

  virtual ~Base() {};
};
}  // namespace EDD::Managers