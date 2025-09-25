#pragma once

#include <shaderc/shaderc.hpp>
#include <string>
#include <unordered_map>
#include <vector>

#include "EngineData/shader_source.h"
namespace EDD {

namespace Tools {

/*
класс для компиляции GLSL в SPIR-V используя shaderc
может рекомпилировать шейдеры во время сборки или в рантайме
*/
class ShaderCompiller {
  std::unordered_map<std::string, Tools::ShaderSource>* shaders_map_;  // массив шейдеров

  // рекомпиляция всех шейдеров
  void CompileAllShadersToSpirv() {
    shaderc::Compiler L_compiler;
    shaderc::CompileOptions L_options;

    // Компиляция vertex шейдеров
    for (const auto& [L_shader_path, L_shader_source] : *shaders_map_) {
      shaderc::SpvCompilationResult L_result = L_compiler.CompileGlslToSpv(
          L_shader_source.vec_source, shaderc_vertex_shader, L_shader_path.c_str(), L_options);
    }

    // Компиляция fragment шейдеров
    for (const auto& [L_shader_path, L_shader_source] : *shaders_map_) {
      shaderc::SpvCompilationResult L_result = L_compiler.CompileGlslToSpv(
          L_shader_source.frag_source,
          shaderc_fragment_shader,
          L_shader_path.c_str(),
          L_options);
    }
  }

 public:
  std::vector<std::string> GetVecShaders(const std::string& shader_name) const {
    if (shaders_map_->contains(shader_name)) {
      return {shaders_map_->at(shader_name).vec_source};
    }
    return {};
  }
  std::vector<std::string> GetFragShaders(const std::string& shader_name) const {
    if (shaders_map_->contains(shader_name)) {
      return {shaders_map_->at(shader_name).frag_source};
    }
    return {};
  }
  std::unordered_map<std::string, ShaderSource> GetCompiledVecShaders(const std::string& shader_name) const {
    if (shaders_map_->contains(shader_name)) {
      return {{shader_name, shaders_map_->at(shader_name)}};
    }
    return {};
  }
  std::unordered_map<std::string, ShaderSource> GetCompiledFragShaders(const std::string& shader_name) const {
    if (shaders_map_->contains(shader_name)) {
      return {{shader_name, shaders_map_->at(shader_name)}};
    }
    return {};
  }
};
}  // namespace Tools
}  // namespace EDD
