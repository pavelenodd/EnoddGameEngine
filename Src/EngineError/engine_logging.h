#pragma once

#include <boost/log/core.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>
#include <boost/log/utility/setup/console.hpp>
#include <iostream>
#include <string>

namespace EDD {
namespace LOG {

// ANSI цветовые коды
namespace Colors {
const std::string RESET = "\033[0m";
const std::string RED = "\033[31m";
const std::string GREEN = "\033[32m";
const std::string YELLOW = "\033[33m";
const std::string BLUE = "\033[34m";
const std::string MAGENTA = "\033[35m";
const std::string CYAN = "\033[36m";
const std::string WHITE = "\033[37m";
const std::string BOLD = "\033[1m";
}  // namespace Colors

// Функция для получения цвета по уровню
inline std::string GetColorForLevel(boost::log::trivial::severity_level level) {
  switch (level) {
    case boost::log::trivial::trace:
      return Colors::CYAN;
    case boost::log::trivial::debug:
      return Colors::BLUE;
    case boost::log::trivial::info:
      return Colors::GREEN;
    case boost::log::trivial::warning:
      return Colors::YELLOW;
    case boost::log::trivial::error:
      return Colors::RED;
    case boost::log::trivial::fatal:
      return Colors::RED + Colors::BOLD;
    default:
      return Colors::WHITE;
  }
}

// Настройка логгера
inline void InitLogging() {
  static bool initialized = false;
  if (!initialized) {
    initialized = true;

    // Добавляем стандартные атрибуты
    boost::log::add_common_attributes();
    // Настройка консольного логгера
    boost::log::add_console_log(
        std::cout,
        boost::log::keywords::format = boost::log::expressions::stream
                                       << "["
                                       << boost::log::expressions::attr<
                                              boost::log::trivial::severity_level>(
                                              "Severity")
                                       << "] " << boost::log::expressions::smessage);

    // Устанавливаем фильтр: выводить сообщения уровня trace и выше
    boost::log::core::get()->set_filter(boost::log::trivial::severity >=
                                        boost::log::trivial::trace);
  }
}

// Функторы для логирования с цветами
inline auto Trace = [](const std::string &message) {
  InitLogging();
  std::cout << Colors::CYAN << "[ Trace\t] " << Colors::RESET << message << '\n';
};

inline auto Debug = [](const std::string &message) {
  InitLogging();
  std::cout << Colors::BLUE << "[ Debug\t] " << Colors::RESET << message << '\n';
};

inline auto Info = [](const std::string &message) {
  InitLogging();
  std::cout << Colors::GREEN << "[ Info\t] " << Colors::RESET << message << '\n';
};

inline auto Warning = [](const std::string &message) {
  InitLogging();
  std::cout << Colors::YELLOW << Colors::BOLD << "[ Warning\t] " << Colors::RESET
            << message << '\n';
};

inline auto Error = [](const std::string &message) {
  InitLogging();
  std::cout << Colors::RED << "[ Error\t] " << Colors::RESET << message << '\n';
};

inline auto Fatal = [](const std::string &message) {
  InitLogging();
  std::cout << Colors::RED << Colors::BOLD << "[ Fatal\t] " << Colors::RESET << message
            << '\n';
};

}  // namespace LOG
namespace TEST_LOG {
inline auto Success = [](const std::string &message) {
  LOG::InitLogging();
  std::cout << LOG::Colors::GREEN << LOG::Colors::BOLD << "[ TEST OK\t] "
            << LOG::Colors::RESET << message << '\n';
};

inline auto Failed = [](const std::string &message) {
  LOG::InitLogging();
  std::cout << LOG::Colors::RED << LOG::Colors::BOLD << "[ TEST FAILED\t] "
            << LOG::Colors::RESET << message << '\n';
  throw std::runtime_error(message);  // Генерируем исключение для тестов
};

inline auto Info = [](const std::string &message) {
  LOG::InitLogging();
  std::cout << LOG::Colors::CYAN << "[ TEST Message\t] " << LOG::Colors::RESET << message
            << '\n';
};

}  // namespace TEST_LOG
}  // namespace EDD