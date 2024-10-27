#pragma once

#include "../../GameEntity/EngineEntity/engine_entity.h"
#include "../inputs.h"
class KeyboardInpit : public Inputs {
 private:
  public:
  KeyboardInpit() {}
  ~KeyboardInpit() {}

  void HandlingInputs(GLFWwindow* L_window,
                      int key,
                      int scancode,
                      int action,
                      int mods) override;
};

void KeyboardInpit::HandlingInputs(GLFWwindow* L_window,
                                   int key,
                                   int scancode,
                                   int action,
                                   int mods) {
  if (inputs_key_map_.at(key) == "Exit" && action == GLFW_PRESS) {
    std::cout << "Exit" << std::endl;
  }
}