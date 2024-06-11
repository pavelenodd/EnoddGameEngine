#pragma once
#include "render_data.h"

namespace edd {

/**
 * Основные примитивы фигур для рендера
 */
namespace primitives {

// Базовый класс для рендеринга фигур
class Shape {
 public:
  virtual void draw(edd::Color color, double size) = 0;
  virtual ~Shape() {}
};

// Класс для рендеринга квадрата
class Square : public Shape {
 public:
  void draw(edd::Color color, double size) override {
    GLdouble vertices[] = {
        -0.5f, -0.5f, 0.0f,  // Вершина 1
        0.5f,  -0.5f, 0.0f,  // Вершина 2
        0.5f,  0.5f,  0.0f,  // Вершина 3
        -0.5f, 0.5f,  0.0f   // Вершина 4
    };
    glColor4d(color.red_, color.green_, color.blue_, color.alpha_);
    glEnableClientState(GL_VERTEX_ARRAY);
    glVertexPointer(3, GL_DOUBLE, 0, vertices);
    glDrawArrays(GL_QUADS, 0, 4);
    glDisableClientState(GL_VERTEX_ARRAY);
  }
};

// Класс для рендеринга треугольника
class Triangle : public Shape {
 public:
  void draw(edd::Color color, double size) override {
    GLdouble vertices[] = {
        -0.5f, -0.5f, 0.0f,  // Вершина 1
        0.5f,  -0.5f, 0.0f,  // Вершина 2
        0.0f,  0.5f,  0.0f   // Вершина 3
    };
    glColor4d(color.red_, color.green_, color.blue_, color.alpha_);
    glEnableClientState(GL_VERTEX_ARRAY);
    glVertexPointer(3, GL_DOUBLE, 0, vertices);
    glDrawArrays(GL_TRIANGLES, 0, 3);
    glDisableClientState(GL_VERTEX_ARRAY);
  }
};

// Класс для рендеринга прямоугольника
class Rectangle : public Shape {
 public:
  void draw(edd::Color color, double size) override {
    GLdouble vertices[] = {
        -0.75f, -0.5f, 0.0f,  // Вершина 1
        0.75f,  -0.5f, 0.0f,  // Вершина 2
        0.75f,  0.5f,  0.0f,  // Вершина 3
        -0.75f, 0.5f,  0.0f   // Вершина 4
    };
    glColor4d(color.red_, color.green_, color.blue_, color.alpha_);
    glEnableClientState(GL_VERTEX_ARRAY);
    glVertexPointer(3, GL_DOUBLE, 0, vertices);
    glDrawArrays(GL_QUADS, 0, 4);
    glDisableClientState(GL_VERTEX_ARRAY);
  }
};

// Класс для рендеринга шестиугольника
class Hexagon : public Shape {
 public:
  void draw(edd::Color color, double size) override {
    const int numVertices = 6;  // Шесть вершин для шестиугольника
    GLdouble vertices[numVertices * 3];

    for (int i = 0; i < numVertices; ++i) {
      float angle = 2 * M_PI * i / numVertices;
      vertices[3 * i] = 0.5f * cos(angle);      // x
      vertices[3 * i + 1] = 0.5f * sin(angle);  // y
      vertices[3 * i + 2] = 0.0f;               // z
    }

    glColor4d(color.red_, color.green_, color.blue_, color.alpha_);
    glEnableClientState(GL_VERTEX_ARRAY);
    glVertexPointer(3, GL_DOUBLE, 0, vertices);
    glDrawArrays(GL_POLYGON, 0, numVertices);
    glDisableClientState(GL_VERTEX_ARRAY);
  }
};

// Класс для рендеринга круга
class Circle : public Shape {
 public:
  void draw(edd::Color color, double size) override {
    const int numVertices = 30;  // Множество вершин для плавности круга

    GLdouble vertices[numVertices * 3];

    for (int i = 0; i < numVertices; ++i) {
      float angle = 2 * M_PI * i / numVertices;
      vertices[3 * i] = 0.5f * cos(angle);      // x
      vertices[3 * i + 1] = 0.5f * sin(angle);  // y
      vertices[3 * i + 2] = 0.0f;               // z
    }
    glColor4d(color.red_, color.green_, color.blue_, color.alpha_);
    glEnableClientState(GL_VERTEX_ARRAY);
    glVertexPointer(3, GL_DOUBLE, 0, vertices);
    glDrawArrays(GL_TRIANGLE_FAN, 0, numVertices);
    glDisableClientState(GL_VERTEX_ARRAY);
  }
};
}  // namespace primitives
}