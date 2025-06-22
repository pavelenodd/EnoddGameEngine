#pragma once
// Менеджер рендера игровых объектов в сцене
#include "Engine/delegate.h"
#include "Engine/interface.h"
//

#include "manager_base.h"
namespace EDD::Managers {

class ManagerRender : public Managers::Base {
 private:
  /* data */
 public:
  ManagerRender() { Init(); }
  ~ManagerRender() { FreeResources(); }

  /**
   * @brief Обновление менеджера (в тике)
   *
   */
  virtual void Update() override {};

  /**
   * @brief Инициализация менеджера (При старте)
   */
  virtual void Init() override {};

  /**
   * @brief Освобождение ресурсов
   */
  virtual void FreeResources() override {};
};
}  // namespace EDD::Managers