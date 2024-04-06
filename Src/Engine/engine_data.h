#pragma once
#include <SFML/OpenGL.hpp>
#include <string>
struct VieportSize {
  int height;
  int width;
};

struct EngineSettings {
  VieportSize vieport_size;  //<
  int depth_bits;            //< Bits of the depth buffer
  int stencil_bits;          //< Bits of the stencil buffer
  int antialiasing_level;    //< Level of antialiasing
  int major_version;         //< Major number of the context version to create
  int minor_version;         //< Minor number of the context version to create
  int attribute_flags;       //< The attribute flags to create the context with
  bool sRGB_capable;         //< Whether the context framebuffer is sRGB capable
  bool is_vertical_sync_enabled = false;  //<
  std::string vieport_name = "Viewport";  //<

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
