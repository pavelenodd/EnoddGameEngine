#include <GL/gl.h>

#include <SFML/OpenGL.hpp>
#include <SFML/Window.hpp>
#include <iostream>
#include <list>

// Определение структуры для представления частей змейки и еды
struct Point {
  int x;
  int y;
};

class Snake {
 private:
  std::list<Point> body;
  Point direction;

 public:
  Snake() {
    // Начальная позиция змейки
    body.push_back({10, 10});
    body.push_back({10, 11});
    body.push_back({10, 12});

    // Начальное направление движения
    direction = {0, -1};  // Движение вверх
  }

  void changeDirection(const Point& newDirection) { direction = newDirection; }

  void move() {
    Point head = body.front();
    head.x += direction.x;
    head.y += direction.y;
    body.push_front(head);  // Добавить новую голову в начало списка
    body.pop_back();  // Удалить последний элемент, имитируя движение
  }

  void grow() {
    Point head = body.front();
    head.x += direction.x;
    head.y += direction.y;
    body.push_front(head);
  }

  void draw() {
    glColor3f(0.0f, 1.0f, 0.0f);  // Задать цвет змейки
    for (const Point& p : body) {
      glBegin(GL_QUADS);
      glVertex2f(p.x, p.y);
      glVertex2f(p.x + 1, p.y);
      glVertex2f(p.x + 1, p.y + 1);
      glVertex2f(p.x, p.y + 1);
      glEnd();
    }
  }

  bool checkCollision() {
    Point head = body.front();
    for (auto it = ++body.begin(); it != body.end(); ++it) {
      if (it->x == head.x && it->y == head.y) {
        return true;  // Столкновение с самим собой
      }
    }
    return false;
  }
};

int main() {
  sf::Window window(sf::VideoMode(800, 600), "SFML Snake Game",
                    sf::Style::Default);
  window.setVerticalSyncEnabled(true);

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glOrtho(0, 40, 30, 0, -1, 1);  // Установка ортографической проекции

  Snake snake;
  sf::Clock clock;
  float timer = 0.0f, delay = 0.1f;

  while (window.isOpen()) {
    sf::Event event;
    while (window.pollEvent(event)) {
      if (event.type == sf::Event::Closed) window.close();
    }

    // Обработка управления
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up))
      snake.changeDirection({0, -1});
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down))
      snake.changeDirection({0, 1});
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
      snake.changeDirection({-1, 0});
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
      snake.changeDirection({1, 0});

    // Таймер для управления скоростью движения змейки
    float time = clock.getElapsedTime().asSeconds();
    clock.restart();
    timer += time;

    if (timer > delay) {
      timer = 0.0f;
      snake.move();
      if (snake.checkCollision()) {
        std::cout << "Game Over!" << std::endl;
        break;  // Выход из игры после столкновения
      }
    }

    glClear(GL_COLOR_BUFFER_BIT);
    snake.draw();  // Отрисовка змейки
    window.display();
  }

  return 0;
}
