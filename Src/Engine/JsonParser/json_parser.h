
#ifndef JSON_PARSER_H
#define JSON_PARSER_H

#include <fstream>
#include <iostream>
#include <map>
#include <optional>
#include <string>

#include "nlohmann/json.hpp"

using json = nlohmann::json;

class JsonParser {
 private:
  json data_;
  const std::string file_address_;

 public:
  JsonParser(const std::string& L_file_address)
      : file_address_(L_file_address) {
    if (!data_.empty()) {
      data_.clear();
    }
    std::ifstream file_stream(L_file_address);
    if (file_stream.is_open()) {
      file_stream >> data_;
      file_stream.close();
    } else {
      std::cerr << "Unable to open file: " << L_file_address << std::endl;
    }
  }

  void SaveToFile() {
    std::ofstream file(file_address_);
    file << data_.dump(
        4);  // Форматируем с отступом в 4 пробела для лучшей читаемости
    file.close();
  }
  // Метод для поиска значения в структуре по имени массива и ключа
  template <typename T>
  T GetParameterValue(const std::string& structure_name,
                      const std::string& array_name,
                      const std::string& key_name) const {
    if (data_.find(structure_name) != data_.end()) {
      for (const auto& item : data_[structure_name]) {
        if (item.find(array_name) != item.end()) {
          for (const auto& arr_item : item[array_name]) {
            if (arr_item.find(key_name) != arr_item.end()) {
              return arr_item[key_name].get<T>();
            }
          }
        }
      }
    }
  }
  // Метод для поиска значения по ключу в массиве JSON
  template <typename T>
  T GetParameterValue(const std::string& array_name,
                      const std::string& key_name) const {
    for (const auto& item : data_.at(array_name)) {
      if (item.find(key_name) != item.end()) {
        return item[key_name].get<T>();
      }
    }
  }

    // Метод для получения значения параметра JSON
    template <typename T>
    T GetParameterValue(const std::string& key_name) const {
      return data_.at(key_name).get<T>();
    }
    /**
     * Проверяет, является ли параметр с данным именем массивом.
     * @param key_name Имя параметра для проверки.
     *
     * @return Возвращает true, если параметр является массивом.
     */
    bool IsParametrArray(const std::string& key_name) const {
      return data_.at(key_name).is_array();
    }
    /**
     * .получение пар "ключ-значение" из массива
     * @param arr_name Имя массива
     * @return std::map<std::string, std::string>
     **/
    std::map<std::string, std::string> Get_ParameterValuesPairs(
        const std::string& arr_name) {
      std::map<std::string, std::string> out;
      if (data_.at(arr_name).is_array()) {
        for (const auto& item : data_.at(arr_name)) {
          for (auto it = item.begin(); it != item.end(); ++it) {
            out[it.key()] = it.value().get<std::string>();
          }
        }
      }
      return out;
    }
    // Метод для установки нового значения параметра JSON
    template <typename T>
    void SetParameterValue(const std::string& key_name, const T& new_value) {
      data_[key_name] = new_value;
      SaveToFile();
    }
    template <typename T>
    void SetParameterValue(const std::string& array_name,
                           const std::string& key_name, const T& new_value) {
      for (auto& item : data_.at(array_name)) {
        if (item.find(key_name) != item.end()) {
          item[key_name] = new_value;
        }
      }
      SaveToFile();
    }
    // Метод для поиска значения в структуре по имени массива и ключа
    template <typename T>
    void SetParameterValue(const std::string& structure_name,
                           const std::string& array_name,
                           const std::string& key_name,  //
                           const T& new_value) {
      if (data_.find(structure_name) != data_.end()) {
        for (auto& item : data_[structure_name]) {
          if (item.find(array_name) != item.end()) {
            for (auto& arr_item : item[array_name]) {
              if (arr_item.find(key_name) != arr_item.end()) {
                arr_item[key_name] = new_value;
              }
            }
          }
        }
      }
      SaveToFile();
    }
};

//\Data\engine_settings.json
//\json_parser\json_parser.cpp

#endif  // JSON_PARSER_H