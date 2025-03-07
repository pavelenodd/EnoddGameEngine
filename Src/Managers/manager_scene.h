// Managers/manager_scene.h

#pragma once
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

#include <cassert>
#include <string>

#include "../EngineData/engine_data.h"
#include "manager_base.h"
/**
 * @brief Менеджер отвечающий за создание и обновление сцены
 *
 */
class ManagerScene : public ManagerBase {
 private:
  SDL_Window* window_ = nullptr;
  SDL_Renderer* renderer_ = nullptr;
  EDD::ViewportData viewport_data_;

 public:
  ManagerScene(EDD::ViewportData viewport_data)
      : viewport_data_(viewport_data) {}
  ~ManagerScene() {}

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

    // Отображаем на экране то, что отрисовали
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
                         viewport_data_.w,                      //
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