#pragma once
#include <map>
#include <string>

namespace edd {
struct VieportSize {
  int height;
  int width;
};
namespace settings {

struct InputSettings {
 private:
  std::map<std::string, char> keyboard_input;
  std::map<std::string, char> mouse_input;

 public:
  void Add_Keyboard_Input(const std::string &L_input_name, const char L_input) {
  }
  std::string Get_Keyboard_Input(const char L_input) const {
    return std::to_string(L_input);
  }
};

struct Engine {
  VieportSize vieport_size;  //<
  int depth_bits;            //< Bits of the depth buffer
  int stencil_bits;          //< Bits of the stencil buffer
  int antialiasing_level;    //< Level of antialiasing
  int major_version;         //< Major number of the context version to create
  int minor_version;         //< Minor number of the context version to create
  int attribute_flags;       //< The attribute flags to create the context with
  bool sRGB_capable;         //< Whether the context framebuffer is sRGB capable
  bool is_vertical_sync_enabled = false;  //<
  const char *vieport_name = "Viewport";  //<

  bool is_opengl_debug_on = true;

  sf::ContextSettings Get_ContextSettings() const {
    sf::ContextSettings L_const_settings;
    L_const_settings.antialiasingLevel = antialiasing_level;
    L_const_settings.attributeFlags = attribute_flags;
    L_const_settings.depthBits = depth_bits;
    L_const_settings.stencilBits = stencil_bits;
    L_const_settings.majorVersion = major_version;
    L_const_settings.minorVersion = minor_version;
    L_const_settings.sRgbCapable = sRGB_capable;
    return L_const_settings;
  }
};
}  // namespace settings
}  // namespace edd
