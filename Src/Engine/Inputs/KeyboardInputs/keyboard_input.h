#pragma once

#include "../../GameEntity/EngineEntity/engine_entity.h"
#include "../inputs.h"
class KeyboardInput : public Inputs {
 private:
  public:
   KeyboardInput() {}
   ~KeyboardInput() {}

   void HandlingInputs(GLFWwindow* L_window,
                       const int& L_key,
                       const int& L_scancode,
                       const int& L_action,
                       const int& L_mods) override;
};

void KeyboardInput::HandlingInputs(GLFWwindow* L_window,
                                   const int& L_key,
                                   const int& L_scancode,
                                   const int& L_action,
                                   const int& L_mods) {
  if (L_action == GLFW_PRESS) {
    if (inputs_key_map_.find(L_key) != inputs_key_map_.end()) {
      std::cout << inputs_key_map_[L_key] << std::endl;
    }
  }
}