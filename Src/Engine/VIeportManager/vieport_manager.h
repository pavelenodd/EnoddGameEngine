#pragma once

#include <GLFW/glfw3.h>

#include "Vieport/Window/window.h"
#include "Vieport/vieport.h"
/**
 * \brief Класс для управления окнами
 */
class VieportManager {
 private:
  GameVieport* curr_vieport_;
  /* data */
 public:
  /*Зона конструкторов ========================================= */
  VieportManager() {
    if (!glfwInit()) {
      return;
    }
  }
  ~VieportManager() { glfwTerminate(); }
  /*==========================================================*/
  /**
   * \brief Создает новый вьюпорт
   */
  void CreateVieport(const WindowSettings& L_winow_settings);
  /**
   * \brief Уничтожает текущий вьюпорт
   */
  void DestroyVieport();

  void UpdateVieport();

  /**
   * \brief Возвращает текущий вьюпорт
   */
  GameVieport* Get_CurrVieport();
  GLFWwindow* Get_CurrWindow();
};

void VieportManager::CreateVieport(const WindowSettings& L_winow_settings) {
  curr_vieport_ = new GameVieport(L_winow_settings);
}

void VieportManager::DestroyVieport() {
  delete this->curr_vieport_;
  this->curr_vieport_ = nullptr;
}

GameVieport* VieportManager::Get_CurrVieport() {
  return this->curr_vieport_;
}

GLFWwindow* VieportManager::Get_CurrWindow() {
  return this->curr_vieport_->Get_Window();
}

void VieportManager::UpdateVieport() {}