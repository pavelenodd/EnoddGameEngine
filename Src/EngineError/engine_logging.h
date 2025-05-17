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

// Настройка логгера
inline void InitLogging() {
  static bool initialized = false;
  if (!initialized) {
    initialized = true;

    // Добавляем стандартные атрибуты
    boost::log::add_common_attributes();

    // Настраиваем вывод в консоль
    boost::log::add_console_log(
        std::cout, boost::log::keywords::format =
                       boost::log::expressions::stream
                       << "["
                       << boost::log::expressions::attr<
                              boost::log::trivial::severity_level>("Severity")
                       << "] " << boost::log::expressions::smessage);

    // Устанавливаем фильтр: выводить сообщения уровня info и выше
    boost::log::core::get()->set_filter(boost::log::trivial::severity >=
                                        boost::log::trivial::info);
  }
}

// Функторы для логирования
inline auto Info = [](const std::string &message) {
  InitLogging();  // Инициализация логгера
  BOOST_LOG_TRIVIAL(info) << message;
};

inline auto Debug = [](const std::string &message) {
  InitLogging();  // Инициализация логгера
  BOOST_LOG_TRIVIAL(debug) << message;
};

inline auto Warning = [](const std::string &message) {
  InitLogging();  // Инициализация логгера
  BOOST_LOG_TRIVIAL(warning) << message;
};

inline auto Fatal = [](const std::string &message) {
  InitLogging();  // Инициализация логгера
  BOOST_LOG_TRIVIAL(fatal) << message;
};

}  // namespace LOG
}  // namespace EDD