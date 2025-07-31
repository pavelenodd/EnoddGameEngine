#pragma once

#include <boost/log/core.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>
#include <boost/log/utility/setup/console.hpp>
#include <iostream>
#include <sstream>
#include <string>

namespace EDD {
namespace LOG {
namespace DATA {

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

// Настройка логгера
inline void InitLogger() {
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
class StreamLogger {
 private:
  std::ostringstream stream_;
  std::string prefix_;
  std::string color_;

 public:
  StreamLogger(std::string prefix, std::string color)
      : prefix_(std::move(prefix)), color_(std::move(color)) {}

  ~StreamLogger() {
    EDD::LOG::DATA::InitLogger();
    std::cout << color_ << prefix_ << EDD::LOG::DATA::Colors::RESET << stream_.str()
              << '\n';
  }

  template <typename T>
  StreamLogger& operator<<(const T& value) {
    stream_ << value;
    return *this;
  }
};
}  // namespace DATA

// Перегрузки для строк
inline DATA::StreamLogger Info(const std::string& message = "") {
  return DATA::StreamLogger("[ Info\t] " + message, EDD::LOG::DATA::Colors::GREEN);
}

inline DATA::StreamLogger Debug(const std::string& message = "") {
  return DATA::StreamLogger("[ Debug\t] " + message, EDD::LOG::DATA::Colors::BLUE);
}

inline DATA::StreamLogger Warning(const std::string& message = "") {
  return DATA::StreamLogger("[ Warning\t] " + message, EDD::LOG::DATA::Colors::YELLOW);
}

inline DATA::StreamLogger Error(const std::string& message = "") {
  return DATA::StreamLogger("[ Error\t] " + message, EDD::LOG::DATA::Colors::RED);
}

inline DATA::StreamLogger Fatal(const std::string& message = "") {
  return DATA::StreamLogger("[ Fatal\t] " + message,
                            EDD::LOG::DATA::Colors::RED + EDD::LOG::DATA::Colors::BOLD);
}

// Перегрузки для __FILE__ и __LINE__
inline DATA::StreamLogger Fatal(const char* file, int line = 0) {
  return DATA::StreamLogger("[ Fatal\t] " + std::string(file) + ":" +
                                std::to_string(line) + " ",
                            EDD::LOG::DATA::Colors::RED + EDD::LOG::DATA::Colors::BOLD);
}

inline DATA::StreamLogger Error(const char* file, int line = 0) {
  return DATA::StreamLogger("[ Error\t] " + std::string(file) + ":" +
                                std::to_string(line) + " ",
                            EDD::LOG::DATA::Colors::RED);
}

inline DATA::StreamLogger Warning(const char* file, int line = 0) {
  return DATA::StreamLogger("[ Warning\t] " + std::string(file) + ":" +
                                std::to_string(line) + " ",
                            EDD::LOG::DATA::Colors::YELLOW);
}

inline DATA::StreamLogger Debug(const char* file, int line = 0) {
  return DATA::StreamLogger("[ Debug\t] " + std::string(file) + ":" +
                                std::to_string(line) + " ",
                            EDD::LOG::DATA::Colors::BLUE);
}

inline DATA::StreamLogger Info(const char* file, int line = 0) {
  return DATA::StreamLogger("[ Info\t] " + std::string(file) + ":" +
                                std::to_string(line) + " ",
                            EDD::LOG::DATA::Colors::GREEN);
}

}  // namespace LOG
namespace TEST_LOG {

inline LOG::DATA::StreamLogger Info(const std::string& message = "") {
  return LOG::DATA::StreamLogger("[ Test Info ] " + message + " ",
                                 EDD::LOG::DATA::Colors::WHITE);
}

inline LOG::DATA::StreamLogger Success(const std::string& message = "") {
  return LOG::DATA::StreamLogger("[ Test Success ] " + message,
                                 EDD::LOG::DATA::Colors::GREEN);
}

inline LOG::DATA::StreamLogger Failed(const std::string& message = "",
                                      const int line = 0) {
  return LOG::DATA::StreamLogger(
      "[ Test Failed ] " + message + ":" + std::to_string(line) + " ",
      EDD::LOG::DATA::Colors::RED + EDD::LOG::DATA::Colors::BOLD);
}

}  // namespace TEST_LOG
}  // namespace EDD