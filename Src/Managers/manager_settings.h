#pragma once
#include <fstream>
#include <map>
#include <nlohmann/json.hpp>

#include "../EngineError/engine_logging.h"
#include "manager_base.h"
namespace EDD {
#ifdef DEBUG
namespace Tests {
struct SettingsInspector;
}
#endif
namespace Managers {
enum SettingsType {
  NONE_TYPE = -1,
  ALL_SETTINGS = 0,
  VIEWPORT_SETTINGS = 1,
  AUDIO_SETTINGS = 2,
  GRAPHICS_SETTINGS = 3,
  INPUT_SETTINGS = 4,
  RENDER_SETTINGS = 5

};
/*
  Менеджер настроек отвечает за загрузку и сохранение настроек игры.
      Он может загружать настройки из файла JSON и сохранять их обратно в файл.
      Автоматически проверяет тип данных со схемой, которая содержит тип
  переменной и её диапазон Настройки могут включать параметры окна, аудио и
  графики.
*/
class Settings : public Managers::Base {
#ifdef DEBUG
  friend struct Tests::SettingsInspector;
#endif

 private:
  std::fstream file_stream_;
  std::map<SettingsType, nlohmann::json> settings_map_;
  std::map<SettingsType, std::string> settings_paths_ = {
      {NONE_TYPE, ""},
      {ALL_SETTINGS, ""},
      {VIEWPORT_SETTINGS, "Settings/viewport_settings.jsonc"},
      {AUDIO_SETTINGS, "Settings/audio_settings.jsonc"},
      {GRAPHICS_SETTINGS, "Settings/graphics_settings.jsonc"},
      {INPUT_SETTINGS, "Settings/input_settings.jsonc"},
      {RENDER_SETTINGS, "Settings/render_settings.jsonc"}};

 public:
  explicit Settings() {}

  ~Settings() {
    FreeResources();
    LOG::Debug() << "ManagerSettings destroyed";
  }
  Settings(const Settings &) = delete;
  Settings(Settings &&) = delete;
  Settings &operator=(const Settings &) = delete;
  Settings &operator=(Settings &&) = delete;

 public:
  /**
   * @brief Load settings from a JSON file.
   *
   * @param type The enum type of settings to load.
   * @return bool True if settings were loaded successfully, false otherwise.
   */
  bool LoadSettings(SettingsType type = NONE_TYPE);
  /**
   * @brief Save settings to a JSON file.
   *
   * @param type The enum type of settings to save.
   * @return bool True if settings were saved successfully, false otherwise.
   */
  bool SaveSettings(SettingsType type = NONE_TYPE);
  /**
   * @brief Set the Value object
   *
   * @param type The enum type of settings to modify.
   * @param key The key of the setting to modify.
   * @param value The new value to set.
   * @return true if the value was set successfully, false otherwise.
   */
  bool SetValue(SettingsType type = SettingsType::NONE_TYPE,
                const std::string &key = "",
                const std::any &value = std::any());
  /**
   * @brief Get the Value object
   *
   * @param type The enum type of settings to retrieve.
   * @param key The key of the setting to retrieve.
   * @return std::any The value of the setting, or an empty std::any if not
   * found.
   */
  std::any GetValue(SettingsType type = SettingsType::NONE_TYPE, const std::string &key = "");

  void Update() override;
  void Init(std::vector<std::any> args) override;
  void FreeResources() override;

 private:
  /**
   * @brief Convert any value to JSON
   *
   * @param value The value to convert
   * @param expected_type The expected type of the value
   * @return nlohmann::json The JSON representation of the value
   */
  nlohmann::json AnyToJson(const std::any &value, const std::string &expected_type);
  /**
   * @brief Convert JSON to std::any
   *
   * @param j The JSON value to convert
   * @param expected_type The expected type of the value
   * @return std::any The converted value
   */
  std::any JsonToAny(const nlohmann::json &j, const std::string &expected_type) const;
};
}  // namespace Managers
}  // namespace EDD