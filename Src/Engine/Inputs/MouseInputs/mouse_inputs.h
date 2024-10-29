#pragma once

#include "../inputs.h"

class MouseInpit : public Inputs {
 private:
  /* data */
 public:
  MouseInpit() {}
  ~MouseInpit() {}

  void HandlingInputs(GLFWwindow* L_window,
                      const int& L_key,
                      const int& L_scancode,
                      const int& L_action,
                      const int& L_mods) override;
};

void MouseInpit::HandlingInputs(GLFWwindow* L_window,
                                const int& L_key,
                                const int& L_scancode,
                                const int& L_action,
                                const int& L_mods) {}