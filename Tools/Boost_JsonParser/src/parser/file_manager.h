#pragma once

#include <boost/filesystem.hpp>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>

#include "data.h"
#include "json_parser.h"

namespace b_fs = boost::filesystem;

/**
 * @brief Режим работы менеджера
 * @param READ  режим чтения файла =0
 * @param WRITE режим записи в файл =1
 */
enum ManagerState { READ = 0, WRITE = 1 };

class ParserManager {
 private:
  vector_pairs cash_json_;  // Массив для хранения парсинга
  std::vector<std::string> key_array_;     // Массив ключей
  const std::string file_adress_;          // адрес файла
  ManagerState state_ = ManagerState::READ;  // режим работы менеджера
 public:
 private:
  /**
   * Получение расширения файла
   * @return описание возвращаемого значения
   * @throws ErrorType описание ошибки
   */
  void OpenDirectopy(b_fs::path L_path);

  /**
   * Получение расширения файла
   * @return описание возвращаемого значения
   * @throws ErrorType описание ошибки
   */
  void OpenFile(b_fs::path L_file_path);

  /**
   * @brief Открытие пути файла или папки
   */
  void OpenPath();

  /**
   * @brief Шаблонная функция для печати значения
   */
  static void PrintValue(const auto& value);

 public:
  ParserManager() {};
  ParserManager(const ParserManager&) = delete;
  ParserManager& operator=(const ParserManager&) = delete;
  ParserManager(ParserManager&&) = delete;
  // ^удаление стандартного коструктора,копирования,перемещения,присваивания

  ParserManager(std::string& L_file_adress);

  ~ParserManager() {}
  /**
   * @brief Чтение файла
   * @param L_key_array Массив ключей для парсинга
   */
  void Read(const std::vector<std::string>& L_key_array);

  /** @brief Запись в файл
   * @param L_key_array Массив ключ/значение для записи
   */
  void Write(const vector_pairs& L_key_array);

  /**
   *Получает результат
   *@return вектор пар ключ-значение
   */
  vector_pairs Get_Result() const;

  /**
   * @brief Функция для печати результата
   * @param result Вектор пар ключ-значение
   */
  void PrintResult() const;
};
