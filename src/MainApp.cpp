#include "imgui.h"
#include "imgui_impl_sdl2.h"
#include "imgui_impl_sdlrenderer2.h"
#include <SDL.h>

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

}

MainApp::~MainApp() {

}

void MainApp::Init() {
  InitSDL();
  InitImGui();
}

void MainApp::RunImGui() {
  ImGuiIO& io = ImGui::GetIO(); (void)io;
  ImColor clear_color(m_clear_color);

  if (ImGui::BeginMainMenuBar())
  {
    if (ImGui::BeginMenu("File"))
    {
      if (ImGui::MenuItem("Open", "Ctrl+O")) {
        auto& new_game = m_games.emplace_back(new GameBase());
        new_game->Init(640, 480);
        SDL_GL_MakeCurrent(m_window, m_glcontext);
        new_game->StartThread();
      }
      ImGui::EndMenu();
    }
    ImGui::EndMainMenuBar();
  }

  for (auto it = m_games.begin(); it != m_games.end(); /* NOP */ ) {
    auto &game = *it;
    if (game) {
      if (game->p_open) {
        if (ImGui::Begin(game->title().c_str(), &game->p_open)) {
          ImGui::Image(reinterpret_cast<ImTextureID>(game->tex()), ImVec2(game->window_width(), game->window_height()),
            ImVec2(0,1), ImVec2(1,0), ImColor(255,255,255,255), ImColor(255,255,255,128));
          ImGui::Text("Game %.1f FPS", game->framerate());
        }
        ImGui::End();
      } else {
        game->set_request_stop(true);
      }
    }
    it++;
  }
#if 1
  for (auto it = m_games.begin(); it != m_games.end(); /* NOP */ ) {
    auto &game = *it;
    if (game) {
      if (game->stopped()) {
        printf("stopped\n");
        game->WaitToStop();
        game->Finish();
         SDL_GL_MakeCurrent(m_window, m_glcontext);

        it = m_games.erase(it);
        continue;
      }
    }
    it++;
  }
#endif
  EngineBase::RunImGui();
}

void MainApp::Clean() {
  for (auto &game: m_games) {
    game->set_request_stop(true);
  }

  for (auto &game: m_games) {
    game->WaitToStop();
    game->Finish();
  }
}
