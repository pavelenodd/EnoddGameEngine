// Managers/manager_resourse.h
#pragma once

#include <string>

#include "engine_logging.h"
#include "manager_base.h"
namespace EDD::Managers {
enum ResourceType {
  Mesh = 0,
  Texture = 1,
  Audio = 2,
  Font = 3,
  Model = 4,
  Animation = 5,
  Shader = 6,
  Script = 7,
  Other = 8
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
  std::string resourse_path_;  // основной путь к ресурсам

 public:
  Resource() {}
  ~Resource() {}
  virtual void Update() override {}
  /**
   * @brief Инициализация менеджера ресурсов
   * @param args Список аргументов для инициализации (локальные пути к ресурсам)
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
        case ResourceType::Mesh: {
          L_path = std::any_cast<std::string>(args[i]);
          break;
        }
        case ResourceType::Texture: {
          L_path = std::any_cast<std::string>(args[i]);
          break;
        }
        case ResourceType::Audio: {
          L_path = std::any_cast<std::string>(args[i]);
          break;
        }
        case ResourceType::Font: {
          L_path = std::any_cast<std::string>(args[i]);
          break;
        }
        case ResourceType::Model: {
          L_path = std::any_cast<std::string>(args[i]);
          break;
        }
      };
      if (!LoadResource(L_path)) {
        LOG::Fatal(__FILE__, __LINE__) << "Failed to load resource: " << L_path;
      }
      LOG::Debug() << "Resource loaded: " << L_path << '\n';
    }
    LOG::Info() << "Resource loading completed.";
  }
  virtual void FreeResources() override {}

 private:
  /**
   * @brief Загрузка ресурса по указанному пути
   *
   * @param path Путь к ресурсу
   * @return true Если ресурс успешно загружен
   */
  bool LoadResource(const std::string& path) {
    bool result = true;
    return result;
  }
};
}  // namespace EDD::Managers