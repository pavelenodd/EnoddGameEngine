// Managers/manager_resourse.h
#pragma once

#include <filesystem>
#include <map>
#include <string>
#include <vector>

#include "Items/audio.h"
#include "Items/font.h"
#include "Items/model.h"
#include "Items/texture.h"
#include "manager_base.h"

namespace EDD::Managers {
enum class ResourceType {
  NONE = 0,
  TEXTURE = 1,
  AUDIO = 2,
  FONT = 3,
  MODEL = 4,
};

template <typename T>
struct TrackedResource {
  std::shared_ptr<T> data_;
  bool is_dirty_;  // true - ресурс должен быть обновлён
};

/**
 * @brief Менеджер отвечающий за обработку ресурсов (текстуры , аудио, локальные файлы, ...)
 */
class Resourse : public Managers::Base {
 public:
 private:
  std::string resourse_path_;
  // INFO cold-блок содержит в себе ресурсы которые редко обновляются
  /**
   * @brief string- тег ресурса
   * @brief TrackedResource- структура содержащая в себе указатель на ресурс
   */
  std::map<std::string, TrackedResource<ResourseItems::Texture>> cold_Textures_;
  std::map<std::string, TrackedResource<Audio>> cold_Audio_;
  std::map<std::string, TrackedResource<Font>> cold_Font_;
  std::map<std::string, TrackedResource<Model>> cold_Models_;

  // INFO hot-блок содержит в себе ресурсы которые обновляются в тике
  /**
   * @brief string- тег ресурса
   * @brief TrackedResource- структура содержащая в себе указатель на ресурс
   */
  std::map<std::string, TrackedResource<ResourseItems::Texture>> hot_Textures_;
  std::map<std::string, TrackedResource<Audio>> hot_Audio_;
  std::map<std::string, TrackedResource<Font>> hot_Font_;
  std::map<std::string, TrackedResource<Model>> hot_Models_;

 public:
  Resourse() {}
  ~Resourse() {}
  virtual void Update() override {}

  /**
   * @brief Загружает текстуру в зависимости от её типа
   *
   * @param path путь к ресурсу
   * @param is_cold true - ресурс холодный, false - ресурс горячий
   * @return true - ресурс успешно загружен
   * @return false - ресурс не загружен
   */
  bool LoadTexture(const std::string& path, bool is_cold) {
    if (path.empty()) {
      return false;
    }
    if (path.contains(".")) {
      LoadUnit(path, ResourceType::TEXTURE, is_cold);
    } else {
      LoadFolder(path, ResourceType::TEXTURE, is_cold);
    }
    return true;
  }
  bool LoadAudio(std::string path, bool is_cold) {
    if (path.empty()) {
      return false;
    }
    if (path.contains(".")) {
      LoadUnit(path, ResourceType::AUDIO, is_cold);
    } else {
      LoadFolder(path, ResourceType::AUDIO, is_cold);
    }
    return true;
  }
  bool LoadFont(std::string path, bool is_cold) {
    if (path.empty()) {
      return false;
    }
    if (path.contains(".")) {
      LoadUnit(path, ResourceType::FONT, is_cold);
    } else {
      LoadFolder(path, ResourceType::FONT, is_cold);
    }
    return true;
  }
  bool LoadModel(std::string path, bool is_cold) {
    if (path.empty()) {
      return false;
    }
    if (path.contains(".")) {
      LoadUnit(path, ResourceType::MODEL, is_cold);
    } else {
      LoadFolder(path, ResourceType::MODEL, is_cold);
    }
    return true;
  }

  void UpdateHotResourses() {
    for (auto& texture : hot_Textures_) {
    }
    for (auto& audio : hot_Audio_) {
    }
    for (auto& font : hot_Font_) {
    }
    for (auto& model : hot_Models_) {
    }
  }
  void UpdateColdResourses() {
    for (auto& texture : cold_Textures_) {
    }
    for (auto& audio : cold_Audio_) {
    }
    for (auto& font : cold_Font_) {
    }
    for (auto& model : cold_Models_) {
    }
  }
  void UpdateAllResourses() {
    UpdateHotResourses();
    UpdateColdResourses();
  }
  /**
   * @brief пометить ресурсы на обновление
   *
   * @param resourse_name хеш обновляемого ресурса
   * @param resource_type тип обновляемого рсурса
   */
  void MarkOnUpdateHotResourse(
      std::string resourse_name = "all",
      ResourceType resource_type = ResourceType::NONE) {
    //
    if (resourse_name == "all") {
      for (auto [L_hash, L_resourse] : hot_Textures_) {
        L_resourse.is_dirty_ = true;
      }
      for (auto [L_hash, L_resourse] : hot_Audio_) {
        L_resourse.is_dirty_ = true;
      }
      for (auto [L_hash, L_resourse] : hot_Font_) {
        L_resourse.is_dirty_ = true;
      }
      for (auto [L_hash, L_resourse] : hot_Models_) {
        L_resourse.is_dirty_ = true;
      }
    }
  }

  void FreeHotResourses() {
    for (auto& texture : hot_Textures_) {
    }
    for (auto& audio : hot_Audio_) {
    }
    for (auto& font : hot_Font_) {
    }
    for (auto& model : hot_Models_) {
    }
  }
  void FreeColdResourses() {
    for (auto& texture : cold_Textures_) {
    }
    for (auto& audio : cold_Audio_) {
    }
    for (auto& font : cold_Font_) {
    }
    for (auto& model : cold_Models_) {
    }
  }
  void FreeResourses() {
    FreeHotResourses();
    FreeColdResourses();
  }

 private:
  bool LoadUnit(const std::string& path, ResourceType resourese_type,
                bool is_cold) {
    return true;
  }

  bool LoadFolder(const std::string& path, const ResourceType resourese_type,
                  bool is_cold) {
    for (const auto& L_entry : std::filesystem::directory_iterator(path)) {
      std::string L_full_path = L_entry.path().string();
      std::string L_filename = L_entry.path().stem().string();

      if (L_full_path.empty()) {
        return false;
      }

      switch (resourese_type) {
        case ResourceType::TEXTURE: {
          if (is_cold) {
            hot_Textures_.insert(
                {L_filename,
                 {std::make_shared<ResourseItems::Texture>(L_full_path),
                  false}});

          } else {
            cold_Textures_.insert(
                {L_filename,
                 {std::make_shared<ResourseItems::Texture>(L_full_path),
                  false}});
          }
          break;
        }
        case ResourceType::AUDIO: {
          if (is_cold) {
            hot_Audio_.insert(
                {L_filename, {std::make_shared<Audio>(L_full_path), false}});

          } else {
            cold_Audio_.insert(
                {L_filename, {std::make_shared<Audio>(L_full_path), false}});
          }
          break;
        }
        case ResourceType::FONT: {
          if (is_cold) {
            hot_Font_.insert(
                {L_filename, {std::make_shared<Font>(L_full_path), false}});

          } else {
            cold_Font_.insert(
                {L_filename, {std::make_shared<Font>(L_full_path), false}});
          }
          break;
        }
        case ResourceType::MODEL: {
          if (is_cold) {
            hot_Models_.insert(
                {L_filename, {std::make_shared<Model>(L_full_path), false}});

          } else {
            cold_Models_.insert(
                {L_filename, {std::make_shared<Model>(L_full_path), false}});
          }
          break;
        }
        default: {
          return false;
          break;
        }
      }
    }
    return true;
  }

  virtual void Init() override {}
  virtual void FreeResources() override {}
};
}  // namespace EDD::Managers