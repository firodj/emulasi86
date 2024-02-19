#include "imgui.h"
#include "imgui_impl_sdl2.h"
#include "imgui_impl_sdlrenderer2.h"

#include "MainApp.h"

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

    EngineBase::RunImGui();
}