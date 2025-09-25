#pragma once
#include <cstdint>
#include <string>
#include <unordered_map>
#include <vector>
namespace EDD {
namespace Managers {}

namespace Tools {
/*
класс для компиляции GLSL в SPIR-V используя shaderc
может рекомпилировать шейдеры во время сборки или в рантайме
*/
class ShaderCompiller {
  std::string vertex_shader_path_;
  std::string fragment_shader_path_;

  // кэш скомпилированных шейдеров
  std::unordered_map<std::string, std::vector<uint32_t>> compiled_vec_shaders_;
  std::unordered_map<std::string, std::vector<uint32_t>> compiled_frag_shaders_;

  // исходные коды шейдеров
  std::unordered_map<std::string, std::string> shader_sources_;

  // рекомпиляция всех шейдеров
  void CompileAllShadersToSpirv() {}

 public:
  ShaderCompiller(const std::string& base_path) {
    vertex_shader_path_ = base_path + "Vertex";
    fragment_shader_path_ = base_path + "Fragment";
  }
};
}  // namespace Tools
}  // namespace EDD
