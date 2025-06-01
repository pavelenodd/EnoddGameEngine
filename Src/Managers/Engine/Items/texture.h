// Texture.h
#pragma once

#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
#include <string>
namespace EDD::Managers {
namespace ResourseItems {
class Texture {
 private:
  SDL_Texture* sdl_texture_ = nullptr;   // Указатель на текстуру
  SDL_FRect position_ = {0, 0, 64, 64};  // Позиция и размеры
 public:
  Texture(const std::string& path) {
    SDL_Surface* surface = IMG_Load(path.c_str());
    if (surface == nullptr) {
      // Обработка ошибки загрузки текстуры
      return;
    }
  }
  ~Texture() {}
};
}  // namespace ResourseItems
}  // namespace EDD::Managers