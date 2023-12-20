#include <glad/glad.h>

#include <iostream>

#include "GLFW/glfw3.h"
//"GLFW/glfw3.h" так чтобы всегда был позже glad
using namespace std;

GLfloat point[] = {
    0.0, 0.5, 0.0,
    //^0.0 по х, 0.5 по У, 0.0 по z;
    0.5, -0.5, 0.0,
    //^0,5 по х, -0,5 по У, 0,0 по z
    -0.5, -0.5, 0.0
    //^-0,5 по х, -0,5 по У, 0,0 по z;
};
// нормированые координаты вертекса
GLfloat colors[] = {
    1.0, 0.0, 0.0,
    // красный цвет первой точки
    0.0, 1.0, 0.0,
    // зелёный цвет первой точки
    0.0, 0.0, 1.0
    // синий цвет первой точки
};

const char* vertex_shader =
    // отрисовка треугольника
    "#version 460\n"
    // макрос использования версии openGL
    "layout(location = 0) in vec3 vertex_position;"
    // "vec3 vertex_position" фигура(вертекс) из 3 значений(цифра3 в vec3)
    // "in" -входной параметр
    "layout(location = 1) in vec3 vertex_color;"
    "out vec3 color;"
    "void main() {"
    "   color = vertex_color;"
    "   gl_Position = vec4(vertex_position, 1.0);"
    "}";

const char* fragment_shader =
    // раскраска треугольника
    "#version 460\n"
    "in vec3 color;"
    "out vec4 frag_color;"
    "void main() {"
    "   frag_color = vec4(color, 1.0);"
    "}";

int window_size_x = 640;
int window_size_y = 480;

void GlfW_WindowSizeCallback(GLFWwindow* ptr_window, int x_size, int y_size) {
  // обновление размера окна
  window_size_x = x_size;
  window_size_y = y_size;
  glViewport(0, 0, window_size_x, window_size_y);
  //^ показывает в какой области окна происходит отрисовка.
  // если window_size_x/2(половина значения window_size_x) по отприсовка
  // происходит в половине окна по оси Х(влевой или вправой)
}

void GlfW_KeyCallback(GLFWwindow* ptr_window, int key, int scancode, int action,
                      int mode) {
  // закрытие окна на клавишу escape
  if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
    glfwSetWindowShouldClose(ptr_window, GL_TRUE);
  }
}
int main() {
  GLFWwindow* ptr_window;

  /* инициализации библиотеки glfw */
  if (!glfwInit()) {
    cerr << "glfw not init \n";
    return -1;
  }

  // указание версии glfw в использовании программы
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
  // использование обратной совместимости разных версий opengl
  glfwWindowHint(GLFW_OPENGL_ANY_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  /* Create a windowed mode window and its OpenGL context */
  ptr_window = glfwCreateWindow(window_size_x, window_size_y, "Game Engine",
                                nullptr, nullptr);
  if (!ptr_window) {
    cerr << "glfwCreateWindow falied \n";
    glfwTerminate();
    return -1;
  }
  // изменение размеров окна
  glfwSetWindowPosCallback(ptr_window, GlfW_WindowSizeCallback);
  glfwSetKeyCallback(ptr_window, GlfW_KeyCallback);
  /* Make the window's context current */
  glfwMakeContextCurrent(ptr_window);

  if (!gladLoadGL()) {
    cerr << "don`t load GLAD" << endl;
  }
  cout << "Renderer: " << glGetString(GL_RENDERER) << endl;
  cout << "OpenGL version: " << glGetString(GL_VERSION) << endl;

  glClearColor(1, 1, 0, 1);

  GLuint vs = glCreateShader(GL_VERTEX_SHADER);
  //^создать вертекс(фигуру)
  glShaderSource(vs, 1, &vertex_shader, nullptr);
  //^передать исходный код,количество строк,массив строк,массив длин
  glCompileShader(vs);
  //^скомпилировать вертекс(фигуру)

  GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(fs, 1, &fragment_shader, nullptr);
  glCompileShader(fs);

  GLuint shader_program = glCreateProgram();
  //^создание шейдерной программы
  glAttachShader(shader_program, vs);
  glAttachShader(shader_program, fs);
  //^добавление шейдеров в программу
  glLinkProgram(shader_program);
  //^линковка шейдеров

  glDeleteShader(vs);
  glDeleteShader(fs);
  //^удаление вертексного и фрагментарных шейдеров

  // передача позиции шейдера в буффер видеокарты
  GLuint points_vbo = 0;
  glGenBuffers(1, &points_vbo);
  //^генерация числа буфферов , ссылка на значения буффера/ров
  glBindBuffer(GL_ARRAY_BUFFER, points_vbo);
  //^подключение буффера и сделать его текущим
  glBufferData(GL_ARRAY_BUFFER, sizeof(point), point, GL_STATIC_DRAW);
  // передать_буффер,резмер_буффера_(в_байтах),указатель_на_буффер,подсказка(куда_записывать_на_видеокарте)

  // передача цвета шейдера в буффер видеокарты
  GLuint colors_vbo = 0;
  glGenBuffers(1, &colors_vbo);
  glBindBuffer(GL_ARRAY_BUFFER, colors_vbo);
  glBufferData(GL_ARRAY_BUFFER, sizeof(colors), colors, GL_STATIC_DRAW);

  GLuint vao = 0;
  glGenVertexArrays(1, &vao);
  glBindVertexArray(vao);

  glEnableVertexAttribArray(0);
  glBindBuffer(GL_ARRAY_BUFFER, points_vbo);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

  glEnableVertexAttribArray(1);
  glBindBuffer(GL_ARRAY_BUFFER, colors_vbo);
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

  /* Loop until the user closes the window */
  while (!glfwWindowShouldClose(ptr_window)) {
    /* Render here */
    glClear(GL_COLOR_BUFFER_BIT);

    // отрисовка шейдера
    glUseProgram(shader_program);
    glBindVertexArray(vao);
    glDrawArrays(GL_TRIANGLES, 0, 3);

    /* Swap front and back buffers */
    glfwSwapBuffers(ptr_window);

    /* Poll for and process events */
    glfwPollEvents();
  }

  glfwTerminate();
  return 0;
}