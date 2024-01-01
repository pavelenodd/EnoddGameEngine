#pragma once

#include <../../../Libs/nlohmann/json.hpp>
#include <SFML/OpenGL.hpp>
#include <SFML/Window.hpp>
#include <iostream>

#include "../FileManager/Json_Parser/JsonParser.h"
using json = nlohmann::json;

struct Windows_Size {
  int height;
  int width;
};

class Render {
 private:
  Windows_Size windows_size_;
  sf::Window window_;
  JsonParser json_parser_;

  // получение размеров окна из настроичного файла
  void Set_WindowsSize() {
    json window_size = json_parser_.Get_Info<json>("windows_size");
    windows_size_.height = window_size["height"];
    windows_size_.width = window_size["width"];
  }

  // инициализация окна
  void InitializeWindow() {
    Set_WindowsSize();
    // Создаем окно
    window_.create(sf::VideoMode(windows_size_.width, windows_size_.height),
                   "GameWindow");
    if (!window_.isOpen()) {
      std::cerr << "Failed to create window" << std::endl;
    }
  }
  void RenderScene() {
    while (window_.isOpen()) {
      sf::Event event;
      while (window_.pollEvent(event)) {
        if (event.type == sf::Event::Closed) window_.close();
      }

      // Очистка буфера кадра
      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

      // Здесь надо код рендеринга с использованием OpenGL при 3D

      // Обновление окна
      window_.display();
    }
  }

 public:
  Render()
      : json_parser_(
            "E:/Projects/LernProjects/EnoddGameEngine/src/GameEngine/"
            "SettingsEngine/"
            "render_settings.json") {
    InitializeWindow();
    RenderScene();
  };
  //~Render() {}
};
