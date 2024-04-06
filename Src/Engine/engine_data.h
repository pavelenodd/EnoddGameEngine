#pragma once
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
};
