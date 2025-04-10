#include "mainwidget.hpp"
#include <thread>
#include <mutex>
#include <memory>

#define IMGUI_DEFINE_MATH_OPERATORS
#include "imgui.h"
#include "imgui_internal.h"
#include <SDL.h>

#include "globals.hpp"
#include "kosongg/hscpp_macros.hpp"
#include <emulasi/base/gamebase.hpp>
#include <emulasi/ui/gameemu.hpp>
#include <emulasi/ui/gamedemo.hpp>
#include <emulasi/core/com/a3d.hpp>
#include "emulasi/util/log.hpp"

#ifdef _USE_HSCPP_
#include "kosongg/hscpp_progress.hpp"
#endif

#ifdef _USE_IFD_
#include <ImFileDialog.hpp>
#endif

#include <kosongg/iconsfontawesome6.h>
#include "watched/imcontrol.hpp"

#include <regex>

hscpp_require_include_dir("${projPath}/src")
hscpp_require_include_dir("${projPath}/kosongg/cpp")
hscpp_require_include_dir("${extPath}/imgui-docking")
hscpp_if (use_ifd)
	hscpp_require_include_dir("${extPath}/ifd/src")
hscpp_end()
hscpp_require_include_dir("${extPath}/hscpp/extensions/mem/include")
hscpp_require_include_dir("${buildPath}/ext/sdl2/include-config-debug")
hscpp_require_include_dir("${buildPath}/ext/sdl2/include-config-debug/SDL2")
hscpp_require_include_dir("${buildPath}/ext/sdl2/include")
hscpp_require_include_dir("${buildPath}/ext/sdl2/include/SDL2")
hscpp_require_include_dir("${buildPath}/gladsources/glad_gl_core_33/include")
hscpp_require_include_dir("${extPath}/glm")
hscpp_require_include_dir("${extPath}/fmt/include")

// if debug


// hscpp_require_source("OtherSource.cpp")

hscpp_if (os == "Windows")
	hscpp_require_library("${buildPath}/kosongg/cpp/imgui/Debug/imgui.lib")
	hscpp_if (use_ifd)
		hscpp_require_library("${buildPath}/Debug/ifd.lib")
	hscpp_end()
	//hscpp_require_library("${buildPath}/ext/fmt/Debug/fmtd.lib")
	hscpp_require_library("${buildPath}/ext/hscpp/extensions/mem/Debug/hscpp-mem.lib")
hscpp_elif (os == "Darwin")
	hscpp_require_library("${projPath}/bin/libimgui.dylib")
	//hscpp_require_library("${projPath}/bin/libfmtd.dylib")
	hscpp_if (use_ifd)
		hscpp_require_library("${projPath}/bin/libifd.dylib")
	hscpp_end()
	hscpp_require_library("${buildPath}/ext/hscpp/extensions/mem/Debug/libhscpp-mem.a")
hscpp_elif (os == "Linux")
	hscpp_require_library("${projPath}/bin/libimgui.so")
	//hscpp_require_library("${projPath}/bin/libfmtd.so")
	hscpp_if (use_ifd)
		hscpp_require_library("${projPath}/bin/libifd.so")
	hscpp_end()
	hscpp_require_library("${buildPath}/ext/hscpp/extensions/mem/Debug/libhscpp-mem.a")
hscpp_else()
    // Diagnostic messages can be printed to the build output with hscpp_message.
    hscpp_message("Unknown OS ${os}.")
hscpp_end()

MainWidget::MainWidget()
{
#ifdef _USE_HSCPP_
    auto cb = [this](hscpp::SwapInfo& info) {
		info.Save("showDemoWindow",   m_showDemoWindow);
		info.Save("showToolMetrics",  m_showToolMetrics);
		info.Save("showToolDebugLog", m_showToolDebugLog);
		info.Save("showToolAbout",    m_showToolAbout);
    };

    Hscpp_SetSwapHandler(cb);

	if (Hscpp_IsSwapping()) {
		return;
	}
#endif
	Creating();
}

MainWidget::~MainWidget()
{
#ifdef _USE_HSCPP_
    if (Hscpp_IsSwapping())
    {
        return;
    }
#endif
	Destroying();
}

void MainWidget::Creating()
{
	m_showDemoWindow   = false;
    m_showToolMetrics  = false;
    m_showToolDebugLog = false;
    m_showToolAbout    = false;

	// m_pCustomControl = Globals::Resolve()->pMemoryManager->Allocate<CustomControl>();
}

void MainWidget::Destroying() {}

// https://github.com/tpecholt/imrad/blob/main/src/imrad.cpp
void MainWidget::DockSpaceUI()
{
	Globals *globals = Globals::Resolve();
	ImGuiViewport* viewport = ImGui::GetMainViewport();
	ImGui::SetNextWindowPos(viewport->Pos + ImVec2(0, globals->toolbarSize));
	ImGui::SetNextWindowSize(viewport->Size - ImVec2(0, globals->toolbarSize));
	ImGui::SetNextWindowViewport(viewport->ID);
	ImGuiWindowFlags window_flags = 0
		| ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking
		| ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse
		| ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove
		| ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;

	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
	ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
	if (ImGui::Begin("Master DockSpace", NULL, window_flags))
	{
		ImGuiID dockspace_id = ImGui::GetID("MyDockspace");

		// Save off menu bar height for later.
		globals->menuBarHeight = ImGui::GetCurrentWindow()->MenuBarHeight;

		ImGui::DockSpace(dockspace_id);

		//ImGui::DockBuilderAddNode(dockspace_id, ImGuiDockNodeFlags_PassthruCentralNode | ImGuiDockNodeFlags_DockSpace);
		//ImGui::DockBuilderSetNodeSize(dockspace_id, viewport->Size);

		//ImGui::DockBuilderFinish(dockspace_id);
	}
	ImGui::End();

	ImGui::PopStyleVar(3);
}

void MainWidget::ToolbarUI()
{
	Globals *globals = Globals::Resolve();
//
	ImGuiViewport* viewport = ImGui::GetMainViewport();
	ImGui::SetNextWindowPos(ImVec2(viewport->Pos.x, viewport->Pos.y + globals->menuBarHeight));
	ImGui::SetNextWindowSize(ImVec2(viewport->Size.x, globals->toolbarSize));
	ImGui::SetNextWindowViewport(viewport->ID);

	ImGuiWindowFlags window_flags = 0
		| ImGuiWindowFlags_NoDocking
		| ImGuiWindowFlags_NoTitleBar
		| ImGuiWindowFlags_NoResize
		| ImGuiWindowFlags_NoMove
		| ImGuiWindowFlags_NoScrollbar
		| ImGuiWindowFlags_NoSavedSettings
		;
	ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0);
	ImGui::Begin("TOOLBAR", NULL, window_flags);
	ImGui::PopStyleVar();

	const bool disabled = false;

	ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 3.0f);
	ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(8.0f, 4.0f));
	ImGui::PushItemFlag(ImGuiItemFlags_Disabled, disabled);

	if (disabled) {
		ImGui::PushStyleColor(ImGuiCol_Button, IM_COL32(0xFF,0xFF,0xFF,0x80));
		ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(0x00,0x00,0x00,0x40));
	} else {
		ImGui::PushStyleColor(ImGuiCol_Button, IM_COL32(0x00,0x7A,0xFF,0xFF));
		ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32_WHITE);
	}

	if (globals->pImControl->ColoredButtonV1(ICON_FA_PLAY " Play")) {
		//
	}
	ImGui::SameLine();
	if (globals->pImControl->ColoredButtonV1(ICON_FA_STOP " Stop")) {
		//
	}

	ImGui::PopItemFlag();
	ImGui::PopStyleColor(2);
	ImGui::PopStyleVar(2);

	ImGui::End();
}

void OnRunDemo()
{
	Globals *globals = Globals::Resolve();

	auto& new_game = globals->m_games.emplace_back(new emulasi::GameDemo());
	new_game->init();
	SDL_GL_MakeCurrent(globals->m_sdlWindow, globals->m_glContext);
	new_game->startThread();
}

void OnRunEntry(GameEntry& entry)
{
	Globals *globals = Globals::Resolve();

	emulasi::GameBaseConfig cfg;
	cfg.useSwap = false;
	cfg.fpsTime = 0;
	cfg.vertShaderPath = "./data/1texture.vert";
	cfg.fragShaderPath = "./data/1texture.frag";
	cfg.exePath = entry.path;
	cfg.cwd = entry.cwd;
	cfg.title = entry.name;
	auto& new_game = globals->m_games.emplace_back(new emulasi::GameEmuUI(cfg));

	new_game->init();
	SDL_GL_MakeCurrent(globals->m_sdlWindow, globals->m_glContext);
	new_game->startThread();
}

bool UpdateGame(emulasi::GameBase* game)
{
	Globals *globals = Globals::Resolve();

	if (game->paintUI()) return true;

	if (game->stopped()) {
		std::cout << "Game " << game->title() << " stopped" << std::endl;
		game->waitToStop();
		game->finish();

		SDL_GL_MakeCurrent(globals->m_sdlWindow, globals->m_glContext);
		return false;
	}

	game->setRequestStop(true);
	return true;
}

void MainWidget::Update()
{
    ImGui::SetCurrentContext(Globals::Resolve()->pImGuiContext);

    DockSpaceUI();
    ToolbarUI();

	DemoUI();
	MainMenuUI();
	HsCppUI();

	Globals *globals = Globals::Resolve();

	if (ImGui::BeginMainMenuBar()) {
		if (ImGui::BeginMenu("File")) {
			if (ImGui::MenuItem("Run Demo", "Ctrl+O")) {
				OnRunDemo();
			}

			for (auto &entry: globals->entries_) {
				if (ImGui::MenuItem(entry.name.c_str())) {
					OnRunEntry(entry);
				}
			}

			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("ImGui")) {
			ImGui::MenuItem("Demo", NULL, &m_showDemoWindow);
			ImGui::MenuItem("Metrics/Debugger", NULL, &m_showToolMetrics);
			ImGui::MenuItem("Debug Log", NULL, &m_showToolDebugLog);
			ImGui::Separator();
			ImGui::MenuItem("About Dear ImGui", NULL, &m_showToolAbout);

			ImGui::EndMenu();
		}
		ImGui::EndMainMenuBar();
	}

	for (auto it = globals->m_games.begin(); it != globals->m_games.end();)
	{
		auto &game = *it;
		if (!game) {
			it++; continue;
		}

		if (!UpdateGame(game.get())) {
			it = globals->m_games.erase(it);
			continue;
		}

		it++;
	}
}

void MainWidget::DemoUI() {
  if (m_showDemoWindow)
    ImGui::ShowDemoWindow(&m_showDemoWindow);
  if (m_showToolMetrics)
    ImGui::ShowMetricsWindow(&m_showToolMetrics);
  if (m_showToolDebugLog)
    ImGui::ShowDebugLogWindow(&m_showToolDebugLog);
  if (m_showToolAbout)
    ImGui::ShowAboutWindow(&m_showToolAbout);
}

void MainWidget::MainMenuUI() {
	Globals *globals = Globals::Resolve();

	if (ImGui::BeginMainMenuBar())
	{
		if (ImGui::BeginMenu("File"))
		{
			if (ImGui::MenuItem("New", "CTRL+N")) {}
			if (ImGui::MenuItem("Open", "CTRL+O")) {
#if _USE_IFD_
				ifd::FileDialog::Instance().Open("MultiFileOpenDialog",
					"Open a Text file", "Text file (*.txt;*.md){.txt,.md},.*"
				);
#endif
			}
			ImGui::Separator();
			if (ImGui::MenuItem("Exit", "CTRL+W")) {}
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("Window"))
		{
			ImGui::MenuItem("Demo",    nullptr, &m_showDemoWindow);
			ImGui::MenuItem("Hotswap", nullptr, &globals->showHsCppProgress);
			ImGui::EndMenu();
		}
		ImGui::EndMainMenuBar();
	}
}

void MainWidget::HsCppUI() {
#ifdef _USE_HSCPP_
	auto globals = Globals::Resolve();
	auto p = globals->pHsCppProgress.get();

	if (globals->showHsCppProgress) {
		if (ImGui::Begin("Hot-swap Compiler", &globals->showHsCppProgress)) {
		ImColor color = IM_COL32(
			p->lastCompilingColor[0],
			p->lastCompilingColor[1],
			p->lastCompilingColor[2],
			0xFF
		);
		ImGui::TextColored(
			color,
			"Status %s %.2f ms",
			p->lastCompilingText,
			p->lastElapsedCompileTime.count());
		}
		ImGui::End();
	}
#endif
