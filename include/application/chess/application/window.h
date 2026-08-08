#pragma once

#include <string>
#include <utility>

struct GLFWwindow;

namespace chess {

class Window {
public:
  struct Specification {
    std::string name;
    int width;
    int height;
  };

  Window(const Specification& specification);
  ~Window();

  bool Create();
  void Destroy();
  void SwapBuffers();
  bool ShouldClose() const;
  bool IsMinimized() const;

  GLFWwindow* GetHandle() const;
  std::pair<int, int> GetFramebufferSize() const;

  static std::string_view GetGLSLVersion();

private:
  Specification specification_;
  GLFWwindow* windowHandle_;
};

} // namespace chess