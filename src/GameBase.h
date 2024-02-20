#pragma once
#include <glad/gl.h>

#include <mutex>
#include <condition_variable>

struct SDL_Window;
typedef void *SDL_GLContext;

class GameBase
{
public:
  GameBase();
  virtual ~GameBase();

  virtual int Run();
  void Init(int w, int h);
  void Finish();

  void CompileShader();
  void ConfigureVertex();
  void Render();

  void CreateFramebuffer();

  void set_request_stop(bool value) { request_stop_ = value; }
  GLuint tex();
  bool render_full() { return render_full_; }
  void ClearRenderFull();
  void SwapBuffer();
  float hidpi_x() { return hidpi_x_; }
  float hidpi_y() { return hidpi_y_; }
  int window_width() { return window_width_; }
  int window_height() { return window_height_; }
  virtual std::string title() { return "Game"; }
  void WaitToStop();
  void StartThread();

protected:
  std::thread *m_thread;

  GLuint shader_program_, vao_;
  GLuint fb_, rb_, tex_[2];
  int tex_flip_flop_;
  bool request_stop_;
  int window_width_, window_height_;
  int screen_width_, screen_height_;

  std::mutex mtx_render_full_;
  bool render_full_;
  std::condition_variable cv_render_full_;
  float hidpi_x_, hidpi_y_;

  SDL_Window *window_;
  SDL_GLContext glcontext_;
};
