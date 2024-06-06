#pragma once

#include <GL/gl.h>

namespace edd {

struct Point {
  int x;
  int y;
};

class Render {
 private:
  /* data */
 public:
  Point p;
  /**
   * @brief Интерфейс рендера объектов
   */
  void DrawObjects() {
    // Установка цвета очистки и очистка буферов
    // glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    // glClear(GL_COLOR_BUFFER_BIT);

    // Установка цвета квадрата
    glColor3f(0.0f, 1.0f, 0.0f);

    // Рисование квадрата
    glBegin(GL_QUADS);
    glVertex2f(-0.5f, -0.5f);
    glVertex2f(0.5f, -0.5f);
    glVertex2f(0.5f, 0.5f);
    glVertex2f(-0.5f, 0.5f);
    glEnd();
  };
};

}  // namespace edd