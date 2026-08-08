#include "chess/application/window.h"

#include <GLFW/glfw3.h>

namespace chess {

Window::Window(const Specification& specification)
  : specification_{specification},
    windowHandle_{nullptr} {
}

Window::~Window() {
  Destroy();
}

bool Window::Create() {
  if (windowHandle_) {
    printf("Window has already been created\n");
    return false;
  }
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
  windowHandle_ = glfwCreateWindow(specification_.width, specification_.height,
                                   specification_.name.c_str(), nullptr, nullptr);
  if (windowHandle_ == nullptr) {
    printf("GLFW: CreateWindow failed\n");
    return false;
  }
  glfwMakeContextCurrent(windowHandle_);
  glfwSwapInterval(1); // Enable vsync
  return true;
}

void Window::Destroy() {
  if (windowHandle_) {
    glfwDestroyWindow(windowHandle_);
  }
  windowHandle_ = nullptr;
}

void Window::SwapBuffers() {
  glfwSwapBuffers(windowHandle_);
}

bool Window::ShouldClose() const {
  return glfwWindowShouldClose(windowHandle_) != 0;
}

bool Window::IsMinimized() const {
  return glfwGetWindowAttrib(windowHandle_, GLFW_ICONIFIED) != 0;
}

std::pair<int, int> Window::GetFramebufferSize() const {
  auto displayWidth = 0;
  auto displayHeight = 0;
  glfwGetFramebufferSize(windowHandle_, &displayWidth, &displayHeight);
  return {displayWidth, displayHeight};
}

GLFWwindow* Window::GetHandle() const {
  return windowHandle_;
}

std::string_view Window::GetGLSLVersion() {
  return "#version 130";
}

} // namespace chess