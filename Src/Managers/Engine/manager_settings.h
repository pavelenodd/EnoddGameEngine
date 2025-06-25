#pragma once
#include <fstream>
#include <map>
#include <nlohmann/json.hpp>
#include <string>

#include "manager_base.h"

namespace EDD {
enum SettingsType {
  WINDOW_SETTINGS = 0,
  AUDIO_SETTINGS = 1,
  GRAPHICS_SETTINGS = 2
};

class ManagerSettings : public Managers::Base {
 private:
  nlohmann::json settings_;
  std::map<std::string, SettingsType> settings_map_;

 private:
  /**
   * @brief Проверяет, можно ли открыть файл по указанному пути
   * @param file_path Путь к файлу
   */
  bool IsOpenFile(const std::string& file_path) {
    std::ifstream L_file(file_path);
    if (L_file.fail()) {
      return false;
    }
    L_file.open(file_path);
    if (!L_file.is_open()) {
      return false;
    }
    return true;
  }
  /**
   * @brief Закрывает файл, если он открыт
   * @param file_path Путь к файлу для закрытия
   */
  void CloseFile(const std::string& file_path) {
    std::ifstream L_file(file_path);
    if (L_file.is_open()) {
      L_file.close();
    }
  }

 public:
  ManagerSettings() {}
  ~ManagerSettings() {}
  bool LoadSettings(const std::string& file_path) {return true;}
  bool SaveSettings(const std::string& file_path) {return true;}

  void Update() override {}
  void Init() override {}
  void FreeResources() override {}
};

}  // namespace EDD