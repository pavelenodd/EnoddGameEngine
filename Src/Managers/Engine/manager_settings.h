#pragma once
#include <fstream>
#include <map>
#include <nlohmann/json.hpp>
#include <string>

#include "../EngineError/engine_logging.h"
#include "manager_base.h"

namespace EDD {
enum SettingsType { WINDOW_SETTINGS = 0, AUDIO_SETTINGS = 1, GRAPHICS_SETTINGS = 2 };
/*
  Менеджер настроек отвечает за загрузку и сохранение настроек игры.
      Он может загружать настройки из файла JSON и сохранять их обратно в файл.
      Настройки могут включать параметры окна, аудио и графики.
*/
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
  explicit ManagerSettings() {}

  ~ManagerSettings() {
    FreeResources();
    EDD::LOG::Debug("ManagerSettings destroyed");
  }
  // Удаление конструкторов копирования, присваивания и перемещения
  ManagerSettings(const ManagerSettings&) = delete;
  ManagerSettings(ManagerSettings&&) = delete;
  ManagerSettings& operator=(const ManagerSettings&) = delete;
  ManagerSettings& operator=(ManagerSettings&&) = delete;

  bool LoadSettings(const std::string& file_path) {
    return true;
  }
  bool SaveSettings(const std::string& file_path) {
    return true;
  }

  void Update() override {}
  void Init(std::vector<std::any> args) override {
    EDD::LOG::Debug("ManagerSettings initialized");
  }
  void FreeResources() override {}
};

}  // namespace EDD