// Managers/manager_scene.h

#pragma once
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <SDL3_image/SDL_image.h>

#include <cassert>
#include <string>

#include "../EngineData/engine_data.h"
#include "manager_base.h"
/**
 * @brief Менеджер отвечающий за создание и обновление сцены
 *
 */
namespace EDD::Managers {

class Scene : public Managers::Base {
 private:
  SDL_Window* window_ = nullptr;
  SDL_Renderer* renderer_ = nullptr;
  Data::Viewport viewport_data_;
  bool* is_gameloop_enabled_;
  const Tools::Interface<SDL_Event>*
      event_provider_;  // указатель на интерфейс Inputs

 public:
  Scene(Data::Viewport viewport_data,
        const Tools::Interface<SDL_Event>* event_provider,
        bool* is_gameloop_enabled)
      : viewport_data_(viewport_data),
        event_provider_(event_provider),
        is_gameloop_enabled_(is_gameloop_enabled) {}
  ~Scene() {}

 public:
  /**
   * @brief Обновление Вьюпорта
   *
   */
  virtual void Update() override {
    // Задаём цвет очистки (чёрный)
    SDL_SetRenderDrawColor(renderer_, 0, 0, 0, SDL_ALPHA_OPAQUE);

    // Очистка экрана
    SDL_RenderClear(renderer_);

    // TODO: здесь будут вызовы функций отрисовки объектов сцены

    // Обработка событий ввода (нажатие клавиш)
    if (auto event = event_provider_->Send()) {
      // Если нажата клавиша ESC, то выходим из игрового цикла
      if (event->key.key == SDLK_ESCAPE) {
        *is_gameloop_enabled_ = false;
            }
    }

    SDL_RenderPresent(renderer_);
    }

 private:
  /**
   * @brief Инициализация окна и рендерера.
   */
  virtual void Init() override { CreateScene(); }

  /**
   * @brief Освобождаем ресурсы SDL.
   */
  virtual void FreeResources() override {
    if (renderer_) {
      SDL_DestroyRenderer(renderer_);
      renderer_ = nullptr;
    }

    if (window_) {
      SDL_DestroyWindow(window_);
      window_ = nullptr;
    }

    SDL_Quit();
  }

  /**
   * @brief Создание окна и рендерера.
   */
  bool CreateScene() {
    // Создание окна
    window_ =
        SDL_CreateWindow(viewport_data_.viewport_name.c_str(),  // имя окна
                         viewport_data_.w,                      // размеры окна
                         viewport_data_.h,
                         SDL_WINDOW_VULKAN  // флаги окна
        );

    if (!window_) {
      SDL_Log("Ошибка при создании окна: %s", SDL_GetError());
      return false;
    }

    // Создание рендерера
    renderer_ = SDL_CreateRenderer(window_, nullptr);

    if (!renderer_) {
      SDL_Log("Ошибка при создании рендерера: %s", SDL_GetError());
      SDL_DestroyWindow(window_);
      window_ = nullptr;
      return false;
    }

    return true;
  }
};
}  // namespace EDD::Managers