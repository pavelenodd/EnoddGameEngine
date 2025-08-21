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
  VIEWPORT_SETTINGS = 0

};
/*
  Менеджер настроек отвечает за загрузку и сохранение настроек игры.
      Он может загружать настройки из файла JSON и сохранять их обратно в файл.
      Автоматически проверяет тип данных со схемой, которая содержит тип переменной и её диапазон
      Настройки могут включать параметры окна, аудио и графики.
*/
class Settings : public Managers::Base {
 private:
  std::fstream file_stream_;
  std::map<SettingsType, nlohmann::json> settings_map_;
  std::map<SettingsType, std::string> settings_paths_ = {
      {NONE_TYPE, ""}, {VIEWPORT_SETTINGS, "Settings/Test/viewport_settings.jsonc"}};

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
  bool LoadSettings(SettingsType type = NONE_TYPE) {
    if (type == NONE_TYPE) {
      LOG::Error() << "Settings type is NONE_TYPE, cannot load settings";
      return false;
    }
    if (IsOpenFile(settings_paths_.at(type))) {
      settings_map_[type] = std::move(
          nlohmann::json::parse(file_stream_, nullptr, true, true));

      CloseFile(settings_paths_.at(type));
      LOG::Debug() << "Settings loaded from " << settings_paths_.at(type);
      return true;
    } else {
      LOG::Error() << "Failed to open file: path not found " << settings_paths_.at(type);
      return false;
    }
  }
  bool SaveSettings(const std::string& file_path, SettingsType type = NONE_TYPE) {
    if (type == NONE_TYPE) {
      LOG::Error() << "Settings type is NONE_TYPE, cannot save settings";
      return false;
    }
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

 private:
  /**
   * @brief Проверяет, можно ли открыть файл по указанному пути
   * @param file_path Путь к файлу
   */
  bool IsOpenFile(const std::string& file_path) {
    std::fstream file(file_path, std::ios::in | std::ios::out);
    file_stream_ = std::move(file);
    return file_stream_.is_open();
  }
  /**
   * @brief Закрывает файл, если он открыт
   * @param file_path Путь к файлу для закрытия
   */
  void CloseFile(const std::string& file_path) {
    if (file_stream_.is_open()) {
      file_stream_.close();
    }
  }
};

}  // namespace EDD