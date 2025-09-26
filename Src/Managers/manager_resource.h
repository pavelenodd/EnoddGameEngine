// Managers/manager_resource.h
#pragma once
#ifdef STB_IMAGE_IMPLEMENTATION
#undef STB_IMAGE_IMPLEMENTATION
#endif
#include <stb/stb_image.h>

#include <filesystem>
#include <fstream>
#include <list>
#include <string>
#include <unordered_map>
#include <vector>

#include "EngineData/shader_source.h"
#include "EngineError/engine_logging.h"
#include "Managers/manager_base.h"

namespace EDD::Managers {
enum ResourceType {
  Animation = 0,
  Audio = 1,
  Font = 2,
  Image = 3,
  Mesh = 4,
  Model = 5,
  Script = 6,
  Shader = 7,
  Texture = 8
};

/*
Менеджер ресурсов отвечает
      за загрузку/выгрузку ресурсов в движке.
      за объединение ресурсов в группы (entity+texture, entity+audio и т.д.)
      разграничение доступа к ресурсам (холодные и горячие ресурсы)
      поиск ресурсов по типу (texture, audio, font, model и т.д.)
      поиск ресурсов по тегу/имени (например, найти все текстуры с тегом "player")
      передачу ресурсов менеджеру рендера

*/
class Resource : public Managers::Base {
  private:
  struct ImgData {
    std::string name;
    int width = 0;
    int height = 0;
    int channels = 0;
    stbi_uc* data = nullptr;
  };

  // std::map<std::string, sf::Texture> textures_;  // хранилище текстур
  std::unordered_map<ResourceType, std::string> search_paths_ = {
      {ResourceType::Animation, ""},
      {ResourceType::Audio, ""},
      {ResourceType::Font, ""},
      {ResourceType::Image, ""},
      {ResourceType::Mesh, ""},
      {ResourceType::Model, ""},
      {ResourceType::Script, ""},
      {ResourceType::Shader, "Assets/Shaders/"},
      {ResourceType::Texture, "Assets/Textures/"}};

  std::unordered_map<std::string, ImgData> images_;  // хранилище изображений
  std::unordered_map<std::string, Tools::ShaderSource> shaders_map_;  // хранилище шейдеров

 public:
  Resource() {}
  ~Resource() {}
  virtual void Update() override {}
  /**
   * @brief Initialization of a resource manager
   * @param args List of arguments for initialization (local resource paths)
   */
  // <- [WARNING]
  //       высокая сложность инициализации (50+ строк), множественные ответственности,
  //       использование abort() критично для стабильности, отсутствует обработка исключений
  virtual void Init(std::vector<std::any> args) override {
    LOG::Debug() << "Resource manager initialized.";

    // <- [WARNING]
    //       алгоритмическая сложность O(n*m), где n - типы ресурсов, m - файлы в директории,
    //       блокирующая загрузка всех ресурсов может замедлить инициализацию
    for (const auto& [type, path] : search_paths_) {
      switch (type) {
        case ResourceType::Animation:
          LoadAnimation(path);
          break;
        case ResourceType::Audio:
          LoadAudio(path);
          break;
        case ResourceType::Font:
          LoadFont(path);
          break;
        case ResourceType::Image:
          LoadImage(path);
          break;
        case ResourceType::Mesh:
          LoadMesh(path);
          break;
        case ResourceType::Model:
          LoadModel(path);
          break;
        case ResourceType::Script:
          LoadScript(path);
          break;
        case ResourceType::Shader:
          LoadShader(path);
          break;
        case ResourceType::Texture:
          LoadTexture(path);
          break;
        default:
          EDD::LOG::Warning(__PRETTY_FUNCTION__, __LINE__) << "Unknown resource type.";
          break;
      }
    }

    LOG::Info() << "Resource loading completed.";
  }
  virtual void FreeResources() override {
    for (auto& [name, img] : images_) {
      if (img.data) {
        stbi_image_free(img.data);
        img.data = nullptr;
      }
    }
    images_.clear();
  }

 private:
  void LoadAnimation(const std::string& path) {}
  void LoadAudio(const std::string& path) {}
  void LoadFont(const std::string& path) {}
  void LoadImage(const std::string& path) {}
  void LoadMesh(const std::string& path) {}
  void LoadModel(const std::string& path) {}
  void LoadScript(const std::string& path) {}
  void LoadShader(const std::string& path) {
    namespace fs = std::filesystem;
    if (!fs::exists(path) || !fs::is_directory(path)) {
      LOG::Warning() << "Shader path does not exist or is not a directory: " << path;
      return;
    }
    std::list<std::string> L_file_list;  // список файлов в директории
    for (const auto& L_entry : fs::recursive_directory_iterator(path)) {
      // рекурсивный обход директорий для поиска файлов
      if (L_entry.is_regular_file()) {
        L_file_list.push_back(L_entry.path().string());
      }
    }
    if (L_file_list.empty()) {
      LOG::Warning() << "No shader files found in directory: " << path;
      return;
    }
    for (const auto& L_file_path : L_file_list) {
      if (L_file_path.size() >= 5) {
        std::string L_file_name = NormalizeFileName(L_file_path);
        std::string L_ext;
        size_t dotPos = L_file_path.find_first_of('.');  // ищем первую точку
        if (dotPos != std::string::npos) {
          L_ext = L_file_path.substr(dotPos);  // ".txt.doc"
        } else {
          L_ext = "";  // точек нет
        }

        if (L_ext == ".frag" || L_ext == ".vert" || L_ext == ".frag.spv" ||
            L_ext == ".vert.spv") {
          Tools::ShaderSource L_shader;
          if (L_ext == ".frag") {
            std::ifstream file(L_file_path);
            std::ostringstream buffer;
            buffer << file.rdbuf();
            L_shader.frag_source = buffer.str();
            file.close();
          } else if (L_ext == ".vert") {
            std::ifstream file(L_file_path);
            std::ostringstream buffer;
            buffer << file.rdbuf();
            L_shader.vec_source = buffer.str();
            file.close();
          } else if (L_ext == ".frag.spv") {
            std::ifstream file(L_file_path, std::ios::binary | std::ios::ate);
            std::streamsize size = file.tellg();
            file.seekg(0, std::ios::beg);

            std::vector<uint32_t> buffer(size / sizeof(uint32_t));
            file.read(reinterpret_cast<char*>(buffer.data()), size);
            L_shader.frag_spirv_code = buffer;
            file.close();
          } else if (L_ext == ".vert.spv") {
            std::ifstream file(L_file_path, std::ios::binary | std::ios::ate);
            std::streamsize size = file.tellg();
            file.seekg(0, std::ios::beg);

            std::vector<uint32_t> buffer(size / sizeof(uint32_t));
            file.read(reinterpret_cast<char*>(buffer.data()), size);
            L_shader.vec_spirv_code = buffer;
            file.close();
          }
          L_shader.path = L_file_path;
          shaders_map_.insert({L_file_name, L_shader});
          LOG::Info() << "Loaded shader: " << L_file_name << " from " << L_file_path;
        } else {
          LOG::Warning() << "Unsupported shader file extension: " << L_file_path;
        }
      }
    }
  }

  // <- [WARNING]
  //       отсутствует проверка расширений файлов, может загружать неподходящие файлы,
  //       алгоритмическая сложность O(n) для recursive_directory_iterator,
  //       потенциальные утечки памяти при неудачной загрузке stbi_load
  void LoadTexture(const std::string& path) {
    namespace fs = std::filesystem;
    if (!fs::exists(path) || !fs::is_directory(path)) {
      LOG::Warning() << "Texture path does not exist or is not a directory: " << path;
      return;
    }
    std::list<std::string> L_file_list;  // список файлов в директории
    //! <надо объединить два цикла в один
    for (const auto& L_entry : fs::recursive_directory_iterator(path)) {
      if (L_entry.is_regular_file()) {
        L_file_list.push_back(L_entry.path().string());
      }
    }
    if (L_file_list.empty()) {
      LOG::Warning() << "No texture files found in directory: " << path;
      return;
    }
    //! <надо объединить два цикла в один
    for (const auto& L_file_path : L_file_list) {
      ImgData L_img_data;

      L_img_data.data = stbi_load(
          L_file_path.c_str(), &L_img_data.width, &L_img_data.height, &L_img_data.channels, 0);
      if (!L_img_data.data) {
        LOG::Warning() << "Failed to load texture: " << L_file_path;
        continue;
      }
      L_img_data.name = NormalizeFileName(L_file_path);
      images_.insert({L_file_path, L_img_data});
      //! Сделано для отладки, потом убрать
      // DEBUG
      LOG::Info() << "Loaded texture: " << L_file_path << " ("
                  << "Width: " << L_img_data.width << ", Height: " << L_img_data.height
                  << ", Channels: " << L_img_data.channels << ")";
    }
  }
  /**
   * @brief Normalization of the file name
   *
   * @param path File Path
   * @return std::string Normalized file name
   */
  // <- [WARNING]
  //       отсутствует проверка валидности path, потенциальный std::string::npos при find_last_of,
  //       неэффективные множественные substr операции, не учитывает Windows пути с '\'
  std::string NormalizeFileName(const std::string& path) {
    std::string L_texture_name = path.substr(path.find_last_of('/') + 1);
    L_texture_name = L_texture_name.substr(0, L_texture_name.find_first_of('.'));
    return L_texture_name;
  }
};
}  // namespace EDD::Managers