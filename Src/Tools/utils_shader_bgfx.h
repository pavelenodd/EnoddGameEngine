#pragma once
//
// Вспомогательные функции загрузки шейдеров bgfx (.bin)

#include <bgfx/bgfx.h>

#include <fstream>
#include <string>
#include <vector>

namespace EDD::Tools {

inline bgfx::ShaderHandle Load_Shader_Binary(const char* path) {
  std::ifstream L_if(path, std::ios::binary);
  if (!L_if) return BGFX_INVALID_HANDLE;
  std::vector<char> L_buf((std::istreambuf_iterator<char>(L_if)), {});
  const bgfx::Memory* L_mem = bgfx::copy(L_buf.data(), (uint32_t)L_buf.size());
  return bgfx::createShader(L_mem);
}

inline bgfx::ProgramHandle Create_Program(const char* vs_path, const char* fs_path) {
  auto L_vs = Load_Shader_Binary(vs_path);
  auto L_fs = Load_Shader_Binary(fs_path);
  if (!bgfx::isValid(L_vs) || !bgfx::isValid(L_fs)) return BGFX_INVALID_HANDLE;
  return bgfx::createProgram(L_vs, L_fs, true /* destroy shaders */);
}

}  // namespace EDD::Tools
