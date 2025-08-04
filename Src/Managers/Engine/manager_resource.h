// Managers/manager_resourse.h
#pragma once

#include <SFML/Graphics.hpp>
#include <map>
#include <string>

#include "engine_logging.h"
#include "manager_base.h"
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
 public:
 private:
  std::string resource_path_;  // основной путь к ресурсам
  std::map<std::string, sf::Texture> textures_;  // хранилище текстур

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
    if (args.size() < 5) {
      LOG::Fatal(__FILE__, __LINE__)
          << "Insufficient resource paths provided for initialization.";
    }
    std::string L_path;
    for (int i = 0; i < args.size(); ++i) {
      switch (i) {
        case ResourceType::Animation: {
          L_path = std::any_cast<std::string>(args[i]);
          LoadAnimation(L_path);
          break;
        }
        case ResourceType::Audio: {
          L_path = std::any_cast<std::string>(args[i]);
          LoadAudio(L_path);
          break;
        }
        case ResourceType::Font: {
          L_path = std::any_cast<std::string>(args[i]);
          LoadFont(L_path);
          break;
        }
        case ResourceType::Image: {
          L_path = std::any_cast<std::string>(args[i]);
          LoadImage(L_path);
          break;
        }
        case ResourceType::Mesh: {
          L_path = std::any_cast<std::string>(args[i]);
          LoadMesh(L_path);
          break;
        }
        case ResourceType::Model: {
          L_path = std::any_cast<std::string>(args[i]);
          LoadModel(L_path);
          break;
        }
        case ResourceType::Script: {
          L_path = std::any_cast<std::string>(args[i]);
          LoadScript(L_path);
          break;
        }
        case ResourceType::Shader: {
          L_path = std::any_cast<std::string>(args[i]);
          LoadShader(L_path);
          break;
        }
        case ResourceType::Texture: {
          L_path = std::any_cast<std::string>(args[i]);
          LoadTexture(L_path);
          break;
        }
      };

      LOG::Debug() << "Resource loaded: " << L_path << '\n';
    }
    LOG::Info() << "Resource loading completed.";
  }
  virtual void FreeResources() override {}
  sf::Texture* GetTexture(const std::string& name) {
    return &textures_.at(name);
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

  /**
   * @brief Нормализация имени файла
   *
   * @param path Путь к файлу
   * @return std::string Нормализованное имя файла
   */
  std::string NormalizeFileName(const std::string& path) {
    std::string L_texture_name = path.substr(path.find_last_of('/') + 1);
    L_texture_name = L_texture_name.substr(0, L_texture_name.find_last_of('.'));
    return L_texture_name;
  }

 public:
  //! ^^ Public временно для тестов
  /**
   * @brief Загрузка текстуры
   *
   * @param path Путь к файлу текстуры
   */
  void LoadTexture(const std::string& path) {
    std::string L_texture_name = NormalizeFileName(path);

    sf::Texture L_texture;
    if (L_texture.loadFromFile(path)) {
      textures_.insert({L_texture_name, std::move(L_texture)});
    }
  }
};
}  // namespace EDD::Managers