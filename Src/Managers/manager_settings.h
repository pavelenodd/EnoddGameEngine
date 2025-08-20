#pragma once
#include <any>
#include <fstream>
#include <map>
#include <nlohmann/json.hpp>
#include <string>

#include "../EngineError/engine_logging.h"
#include "manager_base.h"

namespace EDD {
enum SettingsType {
  NONE_TYPE = -1,
  WINDOW_SETTINGS = 0,
  AUDIO_SETTINGS = 1,
  GRAPHICS_SETTINGS = 2
};
/*
  Менеджер настроек отвечает за загрузку и сохранение настроек игры.
      Он может загружать настройки из файла JSON и сохранять их обратно в файл.
      Настройки могут включать параметры окна, аудио и графики.
*/
class Settings : public Managers::Base {
 private:
  nlohmann::json settings_;
  std::map<std::string, SettingsType> settings_map_;

 private:
  /**
   * @brief Проверяет, можно ли открыть файл по указанному пути
   * @param file_path Путь к файлу
   */
  bool IsOpenFile(const std::string& file_path) {
    std::fstream file(file_path, std::ios::in);
    return file.is_open();
  }
  /**
   * @brief Закрывает файл, если он открыт
   * @param file_path Путь к файлу для закрытия
   */
  void CloseFile(const std::string& file_path) {
    std::fstream file(file_path, std::ios::in);
    if (file.is_open()) {
      file.close();
    }
  }

 public:
  explicit Settings() {}

  ~Settings() {
    FreeResources();
    LOG::Debug() << "ManagerSettings destroyed";
  }
  // Удаление лишних конструкторов
  Settings(const Settings&) = delete;
  Settings(Settings&&) = delete;
  Settings& operator=(const Settings&) = delete;
  Settings& operator=(Settings&&) = delete;

 public:
  bool LoadSettings(const std::string& file_path) {
    return IsOpenFile(file_path);
  }
  bool SaveSettings(const std::string& file_path) {
    return IsOpenFile(file_path);
  }

  bool SetValue(const std::string& key, const std::any& value) {
    return true;
  }
  std::any GetValue(const std::string& key) {
    return true;
  }

  void Update() override {}
  void Init(std::vector<std::any> args) override {
    LOG::Debug() << "ManagerSettings initialized";
  }
  void FreeResources() override {}
};

}  // namespace EDD