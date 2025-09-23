// manager_scene.cpp
#include "manager_scene.h"

#include <tuple>

#include "EngineError/engine_logging.h"

using namespace EDD::Managers;

void Scene::Init(std::vector<std::any> args) {
  LOG::Debug() << "Scene::Init called";
  if (args.size() <= 1) {
    LOG::Fatal(__PRETTY_FUNCTION__, __LINE__) << "Scene::Init - insufficient parameters";
    return;
  }
  {  // init viewport params
    std::tuple<std::string, uint16_t, uint16_t> L_viewport_params;
    try {
      L_viewport_params = std::any_cast<std::tuple<const std::string, uint16_t, uint16_t>>(
          args[0]);
    } catch (const std::bad_any_cast&) {
      LOG::Fatal(__PRETTY_FUNCTION__, __LINE__) << "Scene::Init - invalid viewport parameters";
      return;
    }

  std::string title = std::get<0>(L_viewport_params);
  uint16_t width = std::get<1>(L_viewport_params);
  uint16_t height = std::get<2>(L_viewport_params);

  auto viewport = CreateViewport(title, width, height);
  if (!viewport) {
    LOG::Fatal(__PRETTY_FUNCTION__, __LINE__) << "Failed to create initial viewport";
    return;
  }
  viewports_.push_back(viewport);
  LOG::Debug() << "Scene::Init completed successfully";
  }
  {  // init gameloop flag
    try {
      is_gameloop_enabled_ = std::any_cast<bool*>(args[1]);
    } catch (const std::bad_any_cast&) {
      LOG::Fatal(__PRETTY_FUNCTION__, __LINE__) << "Scene::Init - invalid gameloop flag parameter";
      return;
    }
  }

  return;
}

void Scene::Update() {
  glfwPollEvents();
}

void Scene::FreeResources() {
  DestroyAllViewport();
  if (viewports_.empty() && glfw_initialized_) {
    glfwTerminate();
    glfw_initialized_ = false;
  }
}

void Scene::DestroyViewport(const std::string& title) {
  for (auto& it : viewports_) {
    if (it->title == title) {
      glfwDestroyWindow(it->viewport_window);
      delete it;
      viewports_.erase(std::remove(viewports_.begin(), viewports_.end(), it),
                       viewports_.end());
      break;
    }
  }
}

void Scene::DestroyAllViewport() {
  for (auto& it : viewports_) {
    glfwDestroyWindow(it->viewport_window);
    delete it;
  }
  viewports_.clear();
}

EDD::Data::Viewport* Scene::GetViewportRef(const std::string& title) const {
  for (const auto& it : viewports_) {
    if (it->title == title) return it;
  }
  return nullptr;
}

std::vector<EDD::Data::Viewport*> Scene::GetAllViewports() const {
  if (viewports_.empty()) {
    LOG::Warning(__PRETTY_FUNCTION__, __LINE__) << "No viewports available";
    return {};
  }
  return viewports_;
}

EDD::Data::Viewport* Scene::CreateViewport(std::string& title,
                                           uint16_t width,
                                           uint16_t height) {
  if (!glfw_initialized_) {
    if (!glfwInit()) {
      LOG::Fatal(__PRETTY_FUNCTION__, __LINE__) << "Failed to initialize GLFW";
      return nullptr;
    }
    glfw_initialized_ = true;
  }

  // Имена уникализируем по твоей логике
  if (GetViewportRef(title)) {
    title += " (copy)";
  }


  glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

  auto* viewport = new EDD::Data::Viewport();
  if (!viewport) {
    LOG::Fatal(__PRETTY_FUNCTION__, __LINE__) << "Failed to allocate Viewport";
    return nullptr;
  }

  viewport->title = title;
  viewport->w = width;
  viewport->h = height;

  // СОЗДАЁМ ОДИН РАЗ и сохраняем хэндл!
  viewport->viewport_window = glfwCreateWindow(
      viewport->w, viewport->h, viewport->title.c_str(), nullptr, nullptr);

  if (!viewport->viewport_window) {
    LOG::Fatal(__PRETTY_FUNCTION__, __LINE__) << "glfwCreateWindow failed";
    delete viewport;
    return nullptr;
  }

  // подключение glfw колбеки
  glfwSetKeyCallback(viewport->viewport_window, &KeyCallback);
  // glfwSetWindowUserPointer(viewport->viewport_window, this);
  glfwSetWindowCloseCallback(viewport->viewport_window, &WindowCloseCallback);
  // glfwSetWindowFocusCallback(viewport->viewport_window, &WindowFocusCallback);
  // glfwSetCursorPosCallback(viewport->viewport_window, &CursorPosCallback);
  // glfwSetMouseButtonCallback(viewport->viewport_window, &MouseButtonCallback);
  // glfwSetScrollCallback(viewport->viewport_window, &ScrollCallback);
  // glfwSetCharModsCallback(viewport->viewport_window, &CharModsCallback);
  // glfwSetDropCallback(viewport->viewport_window, &DropCallback);

  return viewport;
}

void Scene::KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
  Scene* scene = static_cast<Scene*>(glfwGetWindowUserPointer(window));
  if (scene) {
    // Update key states for simultaneous presses
    if (action == GLFW_PRESS) {
      scene->key_states_[key] = true;
    } else if (action == GLFW_RELEASE) {
      scene->key_states_[key] = false;
    }
    // Keep interface_args_ for backward compatibility or single events
    scene->interface_args_.push_back({key, scancode, action, mods});
  }
}
void Scene::WindowCloseCallback(GLFWwindow* window) {
  EDD::LOG::Debug() << "Window close requested";
  *is_gameloop_enabled_ = false;
}
void Scene::WindowFocusCallback(GLFWwindow* window, int focused) {
  if (focused) {
    std::cout << "[DEBUG] Window gained focus" << std::endl;
  } else {
    std::cout << "[DEBUG] Window lost focus" << std::endl;
  }
}
void Scene::CursorPosCallback(GLFWwindow* window, double x_pos, double y_pos) {
  std::cout << "[DEBUG] Cursor position: (" << x_pos << ", " << y_pos << ")" << std::endl;
}
void Scene::MouseButtonCallback(GLFWwindow* window, int button, int action, int mods) {
  if (action == GLFW_PRESS) {
    std::cout << "[DEBUG] Mouse button pressed: " << button << std::endl;
  } else if (action == GLFW_RELEASE) {
    std::cout << "[DEBUG] Mouse button released: " << button << std::endl;
  }
}
void Scene::ScrollCallback(GLFWwindow* window, double x_offset, double y_offset) {
  std::cout << "[DEBUG] Scroll: (" << x_offset << ", " << y_offset << ")" << std::endl;
};
void Scene::CharModsCallback(GLFWwindow* window, unsigned int codepoint, int mods) {
  std::cout << "[DEBUG] Char input with mods: " << static_cast<char>(codepoint)
            << " mods=" << mods << std::endl;
};
void Scene::DropCallback(GLFWwindow* window, int count, const char** paths) {
  for (int i = 0; i < count; i++) {
    std::cout << "[DEBUG] File dropped: " << paths[i] << std::endl;
  }
};