#pragma once

namespace EDD::Tools::EventTypes {
struct KeyEvent {
  int key;
  int scancode;
  int action;
  int mods;
};

}  // namespace EDD::Tools::EventTypes