#pragma once

#include <GLFW/glfw3.h>
#include <algorithm>
#include <vector>
#include "Vieport/Window/window.h"
#include "Vieport/vieport.h"
/**
 * \brief Класс для управления окнами
 */
class VieportManager {
 private:
  std::vector<GameVieport*> vieports_;
  /* data */
 public:
  VieportManager() {
    if (!glfwInit()) {
      return;
    }
  }
  ~VieportManager() {}

  void CreateVieport(const WindowSettings& L_winow_settings);

  void DestroyVieport(GameVieport* L_curr_vieport);

  void UpdateVieport();
};

void VieportManager::CreateVieport(const WindowSettings& L_winow_settings) {
  vieports_.push_back(new GameVieport(L_winow_settings));
}

void VieportManager::DestroyVieport(GameVieport* L_curr_vieport) {
  if (L_curr_vieport != nullptr && !vieports_.empty()) {
    auto it = std::remove(vieports_.begin(), vieports_.end(), L_curr_vieport);

    (*it)->~GameVieport();
    if (it != vieports_.end()) {
      vieports_.erase(it, vieports_.end());
    }
  }
}