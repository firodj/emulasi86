#pragma once

#include <functional>
#include <memory>

#ifdef _USE_HSCPP_
#include "hscpp/module/GlobalUserData.h"
#include "hscpp/mem/Ref.h"
#include "hscpp/mem/MemoryManager.h"
#endif

struct HsCppProgress;

struct ImGuiContext;
class MainWidget;

namespace kosongg {
	class ImControl;
}

namespace emulasi {
	class GameBase;
};
struct SDL_Window;
typedef void *SDL_GLContext;

struct GameEntry {
	int idx;
	std::string name;
	std::string path;
	std::string cwd;
};

class Globals
{
public:
	static Globals* Resolve() {
#ifdef _USE_HSCPP_
		return hscpp::GlobalUserData::GetAs<Globals>();
#else
		return m_g;
#endif
	}

	ImGuiContext* pImGuiContext{nullptr};
#ifdef _USE_HSCPP_
	std::unique_ptr<HsCppProgress> pHsCppProgress;

	hscpp::mem::MemoryManager *pMemoryManager{nullptr};
	hscpp::mem::UniqueRef<MainWidget> pMainWidget;
	hscpp::mem::UniqueRef<kosongg::ImControl> pImControl;
#else
	std::unique_ptr<MainWidget> pMainWidget;
	std::unique_ptr<kosongg::ImControl> pImControl;
#endif
	// hscpp::mem::UniqueRef<CustomControls> pCustomControls;

	bool showHsCppProgress{true};
	float toolbarSize{50};
	float menuBarHeight{0};

	SDL_Window *  m_sdlWindow{nullptr};
	SDL_GLContext m_glContext{nullptr};

	std::vector<std::unique_ptr<emulasi::GameBase>> m_games;
	std::vector<GameEntry> entries_;

	std::function<std::string(const char*, const char*)> getResourcePath;
#ifndef _USE_HSCPP_
	static void SetGlobalUserData(Globals *g) { m_g = g; }
protected:
	static Globals *m_g;
#endif
