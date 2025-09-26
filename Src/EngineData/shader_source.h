#pragma once
#include <string>
#include <vector>
#include <cstdint>
namespace EDD {

namespace Tools {
/*
Структура для хранения информации о шейдере
*/
struct ShaderSource {
  std::string path;                       // путь к шейдеру
  std::string vec_source;                 // исходный код векторного шейдера
  std::string frag_source;                // исходный код фрагментного шейдера
  std::vector<uint32_t> vec_spirv_code;   // скомпилированный SPIR-V код векторного шейдера
  std::vector<uint32_t> frag_spirv_code;  // скомпилированный SPIR-V код фрагментного шейдера
};
}}