#pragma once

#include <mutex>
#include <thread>
#include <glad/gl.h>
#include <SDL.h>
#include "emulasi/gfx/shader.hpp"
#include "emulasi/gfx/offscreen.hpp"
#include "emulasi/gfx/plainbox.hpp"
#include "gamebasecfg.hpp"

namespace emulasi {

class GameBase
{
public:
	GameBase(GameBaseConfig config = GameBaseConfig());
	virtual ~GameBase();

	virtual void Init();
	virtual void Finish();

	virtual int PreRun() { return 0; };
	virtual int OnRun() { return 0; };
	virtual void PostRun() {};
	virtual void PreSwap() {};
	virtual bool PaintUI();

	void StartThread();
	int Run();

	void SwapBuffer();
	void WaitToStop();

	void RequestStop(bool value);
	bool IsStopped();
	GLuint PresentTexture();

public:
	GameBaseConfig m_config{};
	std::string    m_title{};
	float          m_framerate{};
	bool           m_uiOpen;
	std::thread*   m_thread{};

	bool  m_requestStop{};
	bool  m_isStopped{};
	int   m_windowWidth{};
	int   m_windowHeight{};
	int   m_screenWidth{};
	int   m_screenHeight{};
	float m_hidpiX{};
	float m_hidpiY{};

	SDL_Window*   m_window{};
	SDL_GLContext m_glcontext{};
	ShaderProgram m_shader{};
	Offscreen     m_offscreen{};
};

