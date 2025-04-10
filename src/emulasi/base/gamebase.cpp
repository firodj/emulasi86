#include <SDL.h>
#include <iostream>
#include <string>
#include <chrono>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <fmt/core.h>
#include "imgui.h"

#include "kosongg/engine.hpp"
#include "kosongg/glutil.hpp"
#include "emulasi/base/gamebase.hpp"
#include "emulasi/gfx/plainbox.hpp"

namespace emulasi
{

GameBase::GameBase(GameBaseConfig config):
	m_config(config),
	m_title(config.title),
	m_uiOpen(true)
{
	fmt::print("Game constructed: {}\n", m_title);
}

GameBase::~GameBase()
{
	if (m_thread) {
		delete m_thread;
		m_thread  = {};
	}
	fmt::print("Game destroyed\n");
}

void GameBase::Init()
{
	m_windowWidth = m_config.width;
	m_windowHeight = m_config.height;

	SDL_WindowFlags windowFlags = (SDL_WindowFlags)(SDL_WINDOW_ALLOW_HIGHDPI | SDL_WINDOW_OPENGL );

	windowFlags = (SDL_WindowFlags)(windowFlags | SDL_WINDOW_HIDDEN);

	m_window = SDL_CreateWindow(m_title.c_str(),
		SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, m_windowWidth, m_windowHeight, windowFlags);
	if (m_window == nullptr) {
		fmt::print("Error: SDL_CreateWindow(): {}\n", SDL_GetError());
		return;
	}

	m_glcontext = SDL_GL_CreateContext(m_window);
	if (m_glcontext == nullptr) {
		fmt::print("Error: SDL_GL_CreateContext(): {}\n", SDL_GetError());
		SDL_DestroyWindow(m_window);
		m_window = {};
		return;
	}

	SDL_GL_MakeCurrent(m_window, m_glcontext);

	SDL_GL_GetDrawableSize(m_window, &m_screenWidth, &m_screenHeight);
	m_hidpiX = (float)m_screenWidth / m_windowWidth;
	m_hidpiY = (float)m_screenHeight / m_windowHeight;

	if (m_config.useSwap) {
		if (SDL_GL_SetSwapInterval(1)) {
			fmt::print("Error: SDL_GL_SetSwapInterval(): {}\n", SDL_GetError());
		}
	} else {
		SDL_GL_SetSwapInterval(0);
	}

	if (!m_config.vertShaderPath.empty()) {
		m_shader.LoadFromFile(m_config.vertShaderPath, m_config.fragShaderPath);
		m_shader.Compile();
		m_shader.Activate();
	}

	m_offscreen.Init(m_screenWidth, m_screenHeight);
}

void GameBase::Finish()
{
	SDL_GL_DeleteContext(m_glcontext);
	SDL_DestroyWindow(m_window);
}

GLuint GameBase::PresentTexture()
{
	if (m_offscreen.IsOk()) return m_offscreen.GetTexture();
	return GL_NONE;
}

void GameBase::SwapBuffer()
{
	PreSwap();

	if (m_offscreen.IsOk()) m_offscreen.SwapTexture();

	if (m_config.useSwap) {
		SDL_GL_SwapWindow(m_window);
	}
}

class SyncFrame
{
public:
	SyncFrame(bool useSwap, int fpsTime):
		m_useSwap(useSwap), m_fpsTime(fpsTime)
	{
		Init();
	}
	void Init() {
		m_oldTick    = SDL_GetTicks64();
		m_lastTick   = m_oldTick;
		m_frameCount = 0;
		m_framerate  = 0;
	}
	void Wait() {
		Uint64 currentTick = SDL_GetTicks64();

		if (!m_useSwap && m_fpsTime > 0) {
			int wait = m_fpsTime;
			Uint64 nextTick = m_lastTick + wait;
			if (currentTick <= nextTick) {
				wait = nextTick - currentTick;
			} else {
				wait = 0;
				nextTick = currentTick;
			}
			if (wait > 0) SDL_Delay(wait);
			m_lastTick = nextTick;
		}

		m_frameCount += 1;
		if (currentTick > m_oldTick + 1000) {
			m_framerate = m_frameCount * 1000.0f / (float)(currentTick - m_oldTick);
			m_frameCount = 0;
			m_oldTick = currentTick;
		}
	}

	float FrameRate() { return m_framerate; }

protected:
	Uint64 m_oldTick;
	Uint64 m_lastTick;
	int    m_frameCount;
	bool   m_useSwap;
	int    m_fpsTime;
	float  m_framerate;
};

int GameBase::Run()
{
	fmt::print("Game {} thread start.\n", m_title);
	SDL_GL_MakeCurrent(m_window, m_glcontext);

	int err = PreRun();
	if (err != 0) return err;

	if (m_offscreen.IsOk()) m_offscreen.Activate();

	SyncFrame syncer(m_config.useSwap, m_config.fpsTime);

	while (!m_requestStop) {
		err = OnRun();
		if (err != 0) break;
		syncer.Wait();

		m_framerate = syncer.FrameRate();
	}

	PostRun();

	return err;
}

void GameBase::StartThread()
{
	if (m_thread) return;
	m_thread = new std::thread([&]{
		Run();
		m_isStopped = true;
	});
}

void GameBase::WaitToStop()
{
	m_requestStop = true;
	if (m_thread) {
		m_thread->join();
	}
}

bool GameBase::PaintUI()
{
	if (ImGui::Begin(m_title.c_str(), &m_uiOpen)) {
		ImGui::Image((ImTextureID)(PresentTexture()), ImVec2(m_windowWidth, m_windowHeight),
			ImVec2(0,1), ImVec2(1,0), ImColor(255,255,255,255), ImColor(255,255,255,128));
		ImGui::Text("Game %.1f FPS", m_framerate);
	}
	ImGui::End();

	return m_uiOpen;
}

void GameBase::RequestStop(bool value) { m_requestStop = value; }
bool GameBase::IsStopped() { return m_isStopped; }

}
