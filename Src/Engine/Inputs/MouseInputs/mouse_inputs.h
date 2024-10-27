#pragma once

#include "../inputs.h"

class MouseInpit : public Inputs {
 private:
  /* data */
 public:
  MouseInpit() {}
  ~MouseInpit() {}

  void HandlingInputs(GLFWwindow* L_window,
                      int key,
                      int scancode,
                      int action,
                      int mods) override;
};

void MouseInpit::HandlingInputs(GLFWwindow* L_window,
                                int key,
                                int scancode,
                                int action,
                                int mods) {}