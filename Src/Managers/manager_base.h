// Managers/manager.h
#pragma once
/**
 * @brief Базовый класс менеджеров
 *
 */
class ManagerBase {
 public:
  /**
   * @brief Обновление менеджера
   *
   */
  virtual void Update() = 0;
  /**
   * @brief Инициализация менеджера
   *
   */

  virtual void Init() = 0;
  virtual void FreeResources() = 0;
};