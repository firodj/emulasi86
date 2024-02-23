#pragma once

#include <mutex>

typedef unsigned int GLuint;

class Offscreen {
public:
  Offscreen();

  void Init(int screenWidth, int screenHeight);
  GLuint GetTexture();
  void SwapTexture();
  void Activate();

protected:
  GLuint m_fb, m_rb, m_tex[2];
  int m_tex_flip_flop;
  std::mutex m_mtx_render_full;
};

