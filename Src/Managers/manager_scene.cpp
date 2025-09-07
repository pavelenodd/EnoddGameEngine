#include "manager_scene.h"

#include <cstdio>
#include <tuple>

#include "EngineError/engine_logging.h"
using namespace EDD::Managers;

void Scene::Init(std::vector<std::any> args) {
  LOG::Debug() << "Scene::Init called";
  if (args.size() < 1) {
    LOG::Fatal(__FILE__, __LINE__) << "Scene::Init - insufficient parameters";
    return;
  }
  std::tuple<std::string, uint16_t, uint16_t> L_viewport_params;
  try {
    L_viewport_params = std::any_cast<std::tuple<const std::string, uint16_t, uint16_t>>(
        args[0]);
  } catch (const std::bad_any_cast&) {
    LOG::Fatal(__FILE__, __LINE__) << "Scene::Init - invalid viewport parameters";
    return;
  }

  std::string title = std::get<0>(L_viewport_params);
  int width = std::get<1>(L_viewport_params);
  int height = std::get<2>(L_viewport_params);

  auto viewport = CreateViewport(title, width, height);
  if (!viewport) {
    LOG::Fatal(__FILE__, __LINE__) << "Failed to create initial viewport";
    return;
  }
  viewports_.push_back(viewport);
  LOG::Debug() << "Scene::Init completed successfully";
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
    LOG::Warning(__FILE__, __LINE__) << "No viewports available";
    return {};
  }
  return viewports_;
}

EDD::Data::Viewport* Scene::CreateViewport(std::string& title,
                                           uint16_t width,
                                           uint16_t height) {
  if (!glfw_initialized_) {
    if (!glfwInit()) {
      LOG::Fatal(__FILE__, __LINE__) << "Failed to initialize GLFW";
      return nullptr;
    }
    glfw_initialized_ = true;
  }
  auto existing = GetViewportRef(title);
  if (existing) {
    title += " (copy)";
  }

  EDD::Data::Viewport* viewport = new EDD::Data::Viewport();
  if (!viewport) {
    LOG::Fatal(__FILE__, __LINE__) << "Failed to create viewport";
    return nullptr;
  }

  viewport->title = title;
  viewport->w = width;
  viewport->h = height;

  viewport->viewport_window = glfwCreateWindow(
      viewport->w, viewport->h, viewport->title.c_str(), NULL, NULL);
  if (!viewport->viewport_window) {
    glfwTerminate();
    return nullptr;
  }
  glfwMakeContextCurrent(viewport->viewport_window);
  glfwSetWindowUserPointer(viewport->viewport_window, this);
  glfwSetKeyCallback(viewport->viewport_window, &KeyCallback);
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
    scene->interface_args_ = {key, scancode, action, mods};
    LOG::Debug() << "Key event: " << key << " action: " << action;
  }
}