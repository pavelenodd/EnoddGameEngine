#pragma once

#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/ptree.hpp>
#include <iostream>
#include <string>
#include <variant>
#include <vector>


namespace parser {
#include "data.h"
namespace b_pt = boost::property_tree;

class JsonParser {
 private:
  b_pt::ptree tree_;  // JSON-дерево

  /**
   * Считывает содержимое файла JSON и сохраняет его в дереве свойств.
   * @param file_name Путь к файлу JSON для чтения.
   */
  void ReadJson(const std::string& file_name) {
    try {
      b_pt::read_json(file_name, tree_);
    } catch (const b_pt::json_parser::json_parser_error& e) {
      std::cout << e.what() << '\n';
    }
  }
  /**
   * Записывает содержимое дерева свойств в файл JSON.
   * @param file_name Путь к файлу JSON для записи.
   */
  void WriteJson(const std::string& file_name) {
    try {
      b_pt::write_json(file_name, tree_);
    } catch (const b_pt::json_parser::json_parser_error& e) {
      std::cout << e.what() << '\n';
    }
  }

 public:
  bool Write_ParseJson(const std::string& file_name,
                       const std::vector<pairs>& pairs) {
    bool sucsess = true;
    ReadJson(file_name);

    for (const auto& pair : pairs) {
      auto L_key = std::get<std::string>(pair.second);
      auto child_optional = tree_.get_child_optional(L_key);

      if (!child_optional) {
        std::cerr << "Key: " << L_key << " not found" << std::endl;
        continue;
      }
      const b_pt::ptree& child = child_optional.get();
      if (child.empty()) {
        tree_.put(L_key, std::get<std::string>(pair.second));
      }
    }

    return sucsess;
  }

  value_variant Get_TryValue(const std::string& key) {
    value_variant L_param;
    try {  // Попытка извлечь значение как bool
      L_param = tree_.get<bool>(key);
    } catch (const b_pt::ptree_bad_data&) {
      try {  // Попытка извлечь значение как int
        L_param = tree_.get<int>(key);
      } catch (const b_pt::ptree_bad_data&) {
        try {  // Попытка извлечь значение как double
          L_param = tree_.get<double>(key);
        } catch (const b_pt::ptree_bad_data&) {
          try {  // Попытка извлечь значение как char
            L_param = tree_.get<char>(key);
          } catch (const b_pt::ptree_bad_data&) {
            try {  // Попытка извлечь значение как string
              std::string value = tree_.get<std::string>(key);
              if (value.empty()) {
                L_param = nullptr;  // Пустая строка
              } else {
                L_param = value;  // Непустая строка
              }
            } catch (const b_pt::ptree_bad_data&) {
              L_param = nullptr;  // null значение
            }
          }
        }
      }
    }
    return L_param;
  }

  /**
   * @brief Парсинг JSON-файла
   * @param file_name Имя JSON-файла
   * @param key_array Массив ключей для парсинга
   * @return vector_pairs Вектор пар ключ-значение
   */
  vector_pairs Read_ParseJson(const std::string& file_name,
                              const std::vector<std::string>& key_array) {
    ReadJson(file_name);

    vector_pairs result;
    for (const auto& key : key_array) {
      value_variant param;

      auto child_optional = tree_.get_child_optional(key);
      if (!child_optional) {
        std::cerr << "Key: " << key << " not found" << std::endl;
        continue;
      }
      const b_pt::ptree& child = child_optional.get();
      if (child.empty()) {
        // Если узел пустой, то это может быть string, int, double, char или
        // null
        param = Get_TryValue(key);
      } else {
        // Если узел не пустой, то это объект или массив
        if (tree_.front().first.empty()) {
          // Если первый элемент имеет пустой ключ, то это массив

        } else {
          // В противном случае это объект
        }
      }

      result.emplace_back(key, param);
    }

    return result;
  }
};

}  // namespace parser
