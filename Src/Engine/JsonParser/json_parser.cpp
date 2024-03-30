
#ifndef JSON_PARSER_H
#define JSON_PARSER_H

#include <fstream>
#include <iostream>
#include <string>

#include "nlohmann/json.hpp"

using json = nlohmann::json;

class JsonParser {
 private:
  json data_;

 public:
  JsonParser() = default;

  void ParseFile(const std::string &L_file_address) {
    std::ifstream file_stream(L_file_address);
    if (file_stream.is_open()) {
      file_stream >> data_;
      file_stream.close();
    } else {
      std::cerr << "Unable to open file: " << L_file_address << std::endl;
    }
  }

  // Метод для поиска значения по ключу в массиве объектов JSON
  template <typename T>
  T GetParameterValue(const std::string &array_name,
                      const std::string &key) const {
    for (const auto &item : data_[array_name]) {
      if (item.find(key) != item.end()) {
        return item[key].get<T>();
      }
    }
  }

  // Метод для получения значения параметра JSON-структуры
  template <typename T>
  T GetParameterValue(const std::string &parameter_name) const {
    return data_[parameter_name].get<T>();
  }
};

//\Data\engine_settings.json
//\json_parser\json_parser.cpp

#endif  // JSON_PARSER_H