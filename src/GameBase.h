#pragma once

#include <mutex>
#include <thread>

struct SDL_Window;
typedef void *SDL_GLContext;
typedef unsigned int GLuint;
class Shader;
class Offscreen;
class PlainBox;

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
  virtual void Init(int w, int h);
  virtual void StartThread();

  /**
   * Release resources, delete OpenGL context and SDLWindow.
   */
  void Finish();

  void CompileShader();
  void ConfigureVertex();
  void Render();

  void CreateFramebuffer();
  void SwapBuffer();
  void WaitToStop();

  void set_request_stop(bool value) { m_request_stop = value; }
  bool stopped() { return m_stopped; }
  GLuint tex();
  float hidpi_x() { return m_hidpi_x; }
  float hidpi_y() { return m_hidpi_y; }
  int window_width() { return m_window_width; }
  int window_height() { return m_window_height; }
  std::string title() { return m_title; }
  float framerate() { return m_framerate; }

  bool p_open;

protected:
  std::thread *m_thread;
  std::string m_title;
  GameBaseConfig m_config;

  bool m_request_stop, m_stopped;
  int m_window_width, m_window_height;
  int m_screen_width, m_screen_height;
  float m_hidpi_x, m_hidpi_y;

  SDL_Window *m_window;
  SDL_GLContext m_glcontext;

  float m_framerate;

  static int ID;

  Shader *m_shader;
  Offscreen *m_offscreen;
  PlainBox *m_plain_box;
};
