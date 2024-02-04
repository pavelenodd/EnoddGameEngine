#pragma once

//
#include <GL/gl.h>

#include <../../../Libs/nlohmann/json.hpp>
#include <SFML/OpenGL.hpp>
#include <SFML/Window.hpp>
#include <iostream>

#include "../FileManager/Json_Parser/JsonParser.h"
#include "RenderDATA.h"
#include "Renderer2D.h"
#include "Renderer3D.h"

using json = nlohmann::json;

enum CurrRender {  // текущий рендер
  RENDER_2D,
  RENDER_3D,
};
class Render {
 private:
  Windows_Size windows_size_;
  sf::Window window_;
  JsonParser json_parser_;

  // область инициализации окна
  void Set_WindowsSize() {  // получение размеров окна из настроичного файла
    json window_size = json_parser_.Get_Info<json>("windows_size");
    windows_size_.height = window_size["height"];
    windows_size_.width = window_size["width"];
  }
  void Set_CreateWindow() {  // метод создания окна
    window_.create(sf::VideoMode(windows_size_.width, windows_size_.height),
                   "GameWindow");
    if (!window_.isOpen()) {
      std::cerr << "Failed to create window" << std::endl;
    }
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

  void InitializateWindow() {  // метод инициализации окна
    Set_WindowsSize();
    Set_CreateWindow();
  }

  // область инициализации рендера
  void RenderWindowStream() {}

  void InitializateRender(
      CurrRender curr_render) {  // метод инициализации и подключения рендера

    ContextSettingsOpenGL context_settings_;
    ShadersOpenGL shaders_;
    if (curr_render == RENDER_2D) {
      Renderer_2D renderer_2d_;
      context_settings_ = renderer_2d_.Get_ContextSettings();
      shaders_ = renderer_2d_.Get_Shaders();
      sf::ContextSettings settings(context_settings_.depth_bits,          //
                                   context_settings_.stencil_bits,        //
                                   context_settings_.antialiasing_level,  //
                                   context_settings_.major_version,       //
                                   context_settings_.minor_version);
    } else if (curr_render == RENDER_3D) {
    }
    /*
    передача рендеру контекстных настроек
    */

    /*
    передача рендеру шейдерных настроек
    */

    RenderWindowStream();
  }

  // инициализация окна
  void StartEngine() {
    InitializateWindow();
    InitializateRender(CurrRender::RENDER_2D);
  }

 public:
  Render()
      : json_parser_(
            "E:/Projects/LernProjects/EnoddGameEngine/src/GameEngine/"
            "SettingsEngine/"
            "render_settings.json") {
    StartEngine();
  }
  //~Render() {}
};
