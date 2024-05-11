#include <GL/gl.h>

#include <SFML/OpenGL.hpp>
#include <SFML/Window.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

////////////////////////////////////////////////////////////
/// Entry point of application
////////////////////////////////////////////////////////////
int main() {
  // Create the main window
  sf::Window window(sf::VideoMode(800, 600), "SFML window with OpenGL",
                    sf::Style::Default, sf::ContextSettings(24));
  window.setVerticalSyncEnabled(true);

  // Set the color and depth clear values
  glClearDepth(1.f);
  glClearColor(0.1f, 0.1f, 0.1f, 0.f);
  glEnable(GL_DEPTH_TEST);
  glDepthMask(GL_TRUE);

  // Setup a perspective projection using GLM
  glm::mat4 projection =
      glm::perspective(glm::radians(90.0f), 800.0f / 600.0f, 1.0f, 500.0f);
  glMatrixMode(GL_PROJECTION);
  glLoadMatrixf(glm::value_ptr(projection));

  // Define a 3D triangle
  GLfloat triangleVertices[] = {
      0.0f,  1.0f,  0.0f,  // top
      -1.0f, -1.0f, 0.0f,  // bottom left
      1.0f,  -1.0f, 0.0f   // bottom right
  };

  GLfloat triangleColors[] = {
      1.0f, 0.0f, 0.0f,  // red
      0.0f, 1.0f, 0.0f,  // green
      0.0f, 0.0f, 1.0f   // blue
  };

  float angle = 0.0f;

  // Start the game loop
  while (window.isOpen()) {
    // Process events
    sf::Event event;
    while (window.pollEvent(event)) {
      // Close window: exit
      if (event.type == sf::Event::Closed) window.close();
    }

    // Clear the buffers
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Apply some transformations using GLM
    glm::mat4 modelview =
        glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -3.0f));
    modelview = glm::rotate(modelview, glm::radians(angle),
                            glm::vec3(0.0f, 1.0f, 0.0f));

    glMatrixMode(GL_MODELVIEW);
    glLoadMatrixf(glm::value_ptr(modelview));

    // Draw the triangle
    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_COLOR_ARRAY);
    glVertexPointer(3, GL_FLOAT, 0, triangleVertices);
    glColorPointer(3, GL_FLOAT, 0, triangleColors);
    glDrawArrays(GL_TRIANGLES, 0, 3);
    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_COLOR_ARRAY);

    // Update the window
    window.display();

    // Increment the rotation angle
    angle += 0.5f;
  }

  return EXIT_SUCCESS;
}
