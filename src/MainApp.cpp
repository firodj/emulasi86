#include "imgui.h"
#include "imgui_impl_sdl2.h"
#include "imgui_impl_sdlrenderer2.h"

#include "MainApp.h"
#include "GameBase.h"

#include <thread>
#include <mutex>
#include <memory>

static std::unique_ptr<MainApp> g_mainapp;

static std::mutex g_mainapp_mutex;

MainApp *MainApp::GetInstance(/* dependency */)
{
  std::lock_guard<std::mutex> lock(g_mainapp_mutex);
  if (g_mainapp == nullptr) {
    g_mainapp = std::unique_ptr<MainApp>(new MainApp(/* dependency */));
  }
  return g_mainapp.get();
}

MainApp::MainApp(/* dependency */): kosongg::EngineBase(/* dependency */) {
  m_game = new GameBase();
}

MainApp::~MainApp() {
  if (m_game) delete m_game;
}

void MainApp::Init() {
  InitSDL();
  InitImGui();

  m_game->Init(640, 480);
  m_game->StartThread();
}

void MainApp::RunImGui() {
  ImGuiIO& io = ImGui::GetIO(); (void)io;
  ImColor clear_color(m_clear_color);

  if (ImGui::BeginMainMenuBar())
  {
    if (ImGui::BeginMenu("File"))
    {
      if (ImGui::MenuItem("Open", "Ctrl+O")) {}
      ImGui::EndMenu();
    }
    ImGui::EndMainMenuBar();
  }

  if (m_game) {
    if (ImGui::Begin(m_game->title().c_str())) {
      ImGui::Image(reinterpret_cast<ImTextureID>(m_game->tex()), ImVec2(m_game->window_width(), m_game->window_height()),
        ImVec2(0,1), ImVec2(1,0), ImColor(255,255,255,255), ImColor(255,255,255,128));
      ImGui::Text("Game %.1f FPS", m_game->framerate());
    }
    ImGui::End();
  }

  EngineBase::RunImGui();
}

void MainApp::Clean() {
    m_game->WaitToStop();

    m_game->Finish();
}
