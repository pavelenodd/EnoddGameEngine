#pragma once
#include <string>
using namespace std;

struct Windows_Size {  // размер окна
  int height;
  int width;
};
struct ContextSettingsOpenGL {  // контекстные настройки
  int depth_bits;
  int stencil_bits;
  int antialiasing_level;
  int major_version;
  int minor_version;
};
struct ShadersOpenGL {
 private:                         // настройки шейдеров opengl
  struct VertexShadersSettings {  // вертексный шейдер
    string vertex_shaders;
  };
  struct FragmentShadersSettings {  // фрагментарный шейдер
    string fragment_shaders;
  };

 public:
  VertexShadersSettings vertex_shader_settings;
  FragmentShadersSettings fragment_shader_settings;
};
