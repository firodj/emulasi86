#pragma once

#include <mutex>

struct SDL_Window;
typedef void *SDL_GLContext;
typedef unsigned int GLuint;

struct GameBaseConfig {
  bool use_swap;
  int fps_time;

  GameBaseConfig();
};

class GameBase
{
public:
  GameBase(GameBaseConfig config = GameBaseConfig());
  virtual ~GameBase();

  virtual int Run();
  void Init(int w, int h);
  void Finish();

  void CompileShader();
  void ConfigureVertex();
  void Render();

  void CreateFramebuffer();

  void set_request_stop(bool value) { m_request_stop = value; }
  GLuint tex();

  void SwapBuffer();
  float hidpi_x() { return m_hidpi_x; }
  float hidpi_y() { return m_hidpi_y; }
  int window_width() { return m_window_width; }
  int window_height() { return m_window_height; }
  virtual std::string title() { return "Game"; }
  void WaitToStop();
  void StartThread();
  float framerate() { return m_framerate; }

protected:
  std::thread *m_thread;
  GameBaseConfig m_config;

  GLuint m_shader_program, m_vao;
  GLuint m_fb, m_rb, m_tex[2];
  int m_tex_flip_flop;
  bool m_request_stop;
  int m_window_width, m_window_height;
  int m_screen_width, m_screen_height;
  float m_hidpi_x, m_hidpi_y;

  std::mutex m_mtx_render_full;

  SDL_Window *m_window;
  SDL_GLContext m_glcontext;

  float m_framerate;
};
