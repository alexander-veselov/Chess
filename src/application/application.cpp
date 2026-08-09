#include "chess/application/application.h"

#include "chess/application/ui_constants.h"

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include <GLFW/glfw3.h>

static void GlfwErrorCallback(int error, const char* description) {
  fprintf(stderr, "GLFW Error %d: %s\n", error, description);
}

namespace chess {
namespace {
constexpr auto kBackgroundColor = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
}

Application::Application()
  : window_{nullptr},
    layer_stack_{} {
}

bool Application::Initialize() {
  glfwSetErrorCallback(GlfwErrorCallback);
  if (!glfwInit()) {
    printf("GLFW: Init failed\n");
    return false;
  }

  auto windowSpecification = Window::Specification{};
  windowSpecification.name = "Chess";
  windowSpecification.width = kWindowWidth;
  windowSpecification.height = kWindowHeight;

  window_ = std::make_unique<Window>(windowSpecification);
  if (!window_->Create()) {
    return false;
  }

  IMGUI_CHECKVERSION();
  ImGui::CreateContext();

  const auto contentScale = ImGui_ImplGlfw_GetContentScaleForMonitor(glfwGetPrimaryMonitor());
  ImGui::GetStyle().ScaleAllSizes(contentScale);
  ImGui::GetStyle().FontScaleDpi = contentScale;

  auto& io = ImGui::GetIO();
  io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
  io.IniFilename = nullptr;
  io.FontDefault = io.Fonts->AddFontDefaultVector();

  ImGui::StyleColorsDark();

  ImGui_ImplGlfw_InitForOpenGL(window_->GetHandle(), true);
  ImGui_ImplOpenGL3_Init(Window::GetGLSLVersion().data());

  return true;
}

Application::~Application() {
  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplGlfw_Shutdown();
  ImGui::DestroyContext();

  window_->Destroy();
  glfwTerminate();
}

void Application::Run() {
  while (!window_->ShouldClose()) {
    glfwPollEvents();
    if (window_->IsMinimized()) {
      ImGui_ImplGlfw_Sleep(10);
      continue;
    }

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    for (auto& layer : layer_stack_) {
      layer->OnUIRender();
    }

    ImGui::Render();
    const auto framebufferSize = window_->GetFramebufferSize();
    glViewport(0, 0, framebufferSize.first, framebufferSize.second);
    glClearColor(kBackgroundColor.x * kBackgroundColor.w, kBackgroundColor.y * kBackgroundColor.w,
                 kBackgroundColor.z * kBackgroundColor.w, kBackgroundColor.w);
    glClear(GL_COLOR_BUFFER_BIT);
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    window_->SwapBuffers();
  }
}

void Application::PushLayer(const std::shared_ptr<Layer>& layer) {
  layer_stack_.emplace_back(layer);
}

} // namespace chess