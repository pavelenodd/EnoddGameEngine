#include <iostream>

#include "../FileManager/Json_Parser/JsonParser.h"
#include "RenderDATA.h"

class Renderer_2D {
 private:
  JsonParser json_parser_;

 public:
  ContextSettingsOpenGL Get_ContextSettings() const {
    ContextSettingsOpenGL context_settings;
    context_settings.depth_bits = json_parser_.Get_Info<int>("depth_bits");
    context_settings.stencil_bits = json_parser_.Get_Info<int>("stencil_bits");
    context_settings.antialiasing_level =
        json_parser_.Get_Info<int>("antialiasing_level");
    context_settings.major_version =
        json_parser_.Get_Info<int>("major_version");
    context_settings.minor_version =
        json_parser_.Get_Info<int>("minor_version");
    return context_settings;
  }
  ShadersOpenGL Get_Shaders() const {
    ShadersOpenGL shaders;
    /*
    enter code here
    */
    return shaders;
  }
};