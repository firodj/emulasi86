#include <glad/gl.h>

#include "Offscreen.h"
#include <iostream>
#include "kosongg/GLUtil.h"

Offscreen::Offscreen(): m_tex_flip_flop(0) {

}

void Offscreen::Init(int screenWidth, int screenHeight) {
  // framebuffer configuration
  glGenFramebuffers(1, &m_fb);
  glBindFramebuffer(GL_FRAMEBUFFER, m_fb);

  // create a color attachment texture
  glGenTextures(2, m_tex);

  glBindTexture(GL_TEXTURE_2D, m_tex[0]);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, screenWidth, screenHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  glBindTexture(GL_TEXTURE_2D, m_tex[1]);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, screenWidth, screenHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  int tex_offscreen = (m_tex_flip_flop + 1) % 2;
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_tex[tex_offscreen], 0);

  // create a renderbuffer object for depth and stencil attachment (we won't be sampling these)
  glGenRenderbuffers(1, &m_rb);
  glBindRenderbuffer(GL_RENDERBUFFER, m_rb);
  glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, screenWidth, screenHeight);

  // use a single renderbuffer object for both a depth AND stencil buffer.
  glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, m_rb);
  kosongg::CheckGLError(__FILE__, __LINE__);
  // now actually attach it

  if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
    std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
  }

  glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

GLuint Offscreen::GetTexture() {
  std::unique_lock<std::mutex> lck(m_mtx_render_full);
  return m_tex[ m_tex_flip_flop ];
}

void Offscreen::SwapTexture() {
  std::unique_lock<std::mutex> lck(m_mtx_render_full);

  glFlush();

  m_tex_flip_flop = (m_tex_flip_flop + 1) % 2;
  int tex_offscreen = (m_tex_flip_flop + 1) % 2;

  glBindFramebuffer(GL_FRAMEBUFFER, m_fb);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_tex[tex_offscreen], 0);

  if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
}

void Offscreen::Activate() {
  glBindFramebuffer(GL_FRAMEBUFFER, m_fb);
  kosongg::CheckGLError(__FILE__, __LINE__);
}
