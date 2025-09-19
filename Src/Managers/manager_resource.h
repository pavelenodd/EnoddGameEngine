// Managers/manager_resource.h
#pragma once
#ifdef STB_IMAGE_IMPLEMENTATION
#undef STB_IMAGE_IMPLEMENTATION
#endif
#include <stb/stb_image.h>

#include <filesystem>
#include <list>
#include <string>
#include <unordered_map>
#include <vector>

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
  struct ImgData {
    std::string name;
    int width = 0;
    int height = 0;
    int channels = 0;
    stbi_uc* data = nullptr;
  };

 public:
 private:
  // std::map<std::string, sf::Texture> textures_;  // хранилище текстур
  std::unordered_map<ResourceType, std::string> search_paths_ = {{ResourceType::Animation, ""},
                                                                 {ResourceType::Audio, ""},
                                                                 {ResourceType::Font, ""},
                                                                 {ResourceType::Image, ""},
                                                                 {ResourceType::Mesh, ""},
                                                                 {ResourceType::Model, ""},
                                                                 {ResourceType::Script, ""},
                                                                 {ResourceType::Shader, ""},
                                                                 {ResourceType::Texture, ""}};
  std::unordered_map<std::string, ImgData> images_;  // хранилище изображений

 public:
  Resource() {}
  ~Resource() {}
  virtual void Update() override {}
  /**
   * @brief Initialization of a resource manager
   * @param args List of arguments for initialization (local resource paths)
   */
  virtual void Init(std::vector<std::any> args) override {
    LOG::Debug() << "Resource manager initialized.";
    if (args.size() < 1) {
      EDD::LOG::Fatal(__FILE__, __LINE__)
          << "Resource manager initialization failed. No resource paths provided.";
      abort();
    }
    for (const auto& arg : args) {
      if (arg.type() != typeid(std::pair<ResourceType, std::string>)) {
        EDD::LOG::Fatal(__FILE__, __LINE__)
            << "Resource manager initialization failed. Invalid argument type.";
        continue;
      }

      auto [type, path] = std::any_cast<std::pair<ResourceType, std::string>>(arg);
      search_paths_[type] = path;
    }
    if (search_paths_[ResourceType::Texture].empty()) {
      EDD::LOG::Fatal(__FILE__, __LINE__)
          << "Resource manager initialization failed. No texture path provided.";
      abort();
    }
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
          EDD::LOG::Warning(__FILE__, __LINE__) << "Unknown resource type.";
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
  void LoadShader(const std::string& path) {}

  void LoadTexture(const std::string& path) {
    namespace fs = std::filesystem;
    if (!fs::exists(path) || !fs::is_directory(path)) {
      LOG::Warning() << "Texture path does not exist or is not a directory: " << path;
      return;
    }
    std::list<std::string> file_list;
    for (const auto& entry : fs::recursive_directory_iterator(path)) {
      if (entry.is_regular_file()) {
        file_list.push_back(entry.path().string());
      }
    }
    if (file_list.empty()) {
      LOG::Warning() << "No texture files found in directory: " << path;
      return;
    }
    for (const auto& file_path : file_list) {
      ImgData L_img_data;

      L_img_data.data = stbi_load(
          file_path.c_str(), &L_img_data.width, &L_img_data.height, &L_img_data.channels, 0);
      if (!L_img_data.data) {
        LOG::Warning() << "Failed to load texture: " << file_path;
        continue;
      }
      L_img_data.name = NormalizeFileName(file_path);
      images_.insert({file_path, L_img_data});
      //!
      LOG::Info() << "Loaded texture: " << file_path << " ("
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
  std::string NormalizeFileName(const std::string& path) {
    std::string L_texture_name = path.substr(path.find_last_of('/') + 1);
    L_texture_name = L_texture_name.substr(0, L_texture_name.find_last_of('.'));
    return L_texture_name;
  }
};
}  // namespace EDD::Managers