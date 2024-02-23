// Game.cpp

#include <SDL.h>
#include <iostream>
#include <string>
#include <chrono>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "kosongg/Engine.h"
#include "kosongg/GLUtil.h"
#include "GameBase.h"
#include "Shader.h"
#include "Offscreen.h"
#include "PlainBox.h"

GameBase::GameBase(GameBaseConfig config):
  m_request_stop(false), m_thread(nullptr), m_framerate(0.0), m_stopped(false), m_shader(nullptr),
  m_offscreen(nullptr), m_plain_box(nullptr)
{
  m_config = config;
  p_open = true;

  m_title = "Game " + std::to_string(++GameBase::ID);

  std::cout << "Game constructed: " << m_title << std::endl;
}

GameBase::~GameBase()
{
  if (m_thread) delete m_thread;
  if (m_shader) delete m_shader;
  if (m_offscreen) delete m_offscreen;
  if (m_plain_box) delete m_plain_box;
  std::cout << "Game destroyed" << std::endl;
}

void GameBase::Init(int w, int h)
{
  m_window_width = w;
  m_window_height = h;

  SDL_WindowFlags window_flags = (SDL_WindowFlags)(SDL_WINDOW_ALLOW_HIGHDPI | SDL_WINDOW_OPENGL );

  window_flags = (SDL_WindowFlags)(window_flags | SDL_WINDOW_HIDDEN);

  m_window = SDL_CreateWindow(title().c_str(),
    SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, m_window_width, m_window_height, window_flags);
  if (m_window == nullptr) {
    printf("Error: SDL_CreateWindow(): %s\n", SDL_GetError());
    return;
  }

  m_glcontext = SDL_GL_CreateContext(m_window);
  if (m_glcontext == nullptr) {
    printf("Error: SDL_GL_CreateContext(): %s\n", SDL_GetError());
    SDL_DestroyWindow(m_window);
    m_window = nullptr;
    return;
  }

  SDL_GL_GetDrawableSize(m_window, &m_screen_width, &m_screen_height);
  m_hidpi_x = (float)m_screen_width / m_window_width;
  m_hidpi_y = (float)m_screen_height / m_window_height;

  if (m_config.use_swap) {
    if (SDL_GL_SetSwapInterval(1)) { // Enable vsync
      printf("Error: SDL_GL_SetSwapInterval(): %s\n", SDL_GetError());
    }
  } else {
    SDL_GL_SetSwapInterval(0);
  }
}

void GameBase::Finish()
{
  SDL_GL_DeleteContext(m_glcontext);
  SDL_DestroyWindow(m_window);
}

void GameBase::CreateFramebuffer()
{
  m_offscreen = new Offscreen();
  m_offscreen->Init(m_screen_width, m_screen_height);
}

void GameBase::CompileShader()
{
  m_shader = new Shader();
  m_shader->Load("./data/default.vert", "./data/default.frag");
  m_shader->Compile();
}

void GameBase::ConfigureVertex()
{
  m_plain_box = new PlainBox();
  m_plain_box->Init();
}

void GameBase::Render()
{
  // NOTE: disable me, to draw to offscreen-fb
  //glBindFramebuffer(GL_FRAMEBUFFER, 0);

  glm::mat4 proj = glm::perspective(glm::radians(45.0f), (float)1024 / (float)768, 0.1f, 100.0f);
  glm::mat4 view(1.0);
  view = glm::translate(view, glm::vec3(0.0f, 0.0f, -3.0f));
  glm::mat4 model(1.0);
  model = glm::rotate(model, (float)SDL_GetTicks64()/1000.0f, glm::vec3(0.0f, 0.0f, 1.0f));

  glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT);

  if (m_shader) {
    m_shader->Activate();
    m_shader->SetMatrix4("projection", false, proj);
    m_shader->SetMatrix4("view", false, view);
    m_shader->SetMatrix4("model", false, model);
  }

  if (m_plain_box) m_plain_box->Draw();
}

GLuint GameBase::tex() {
  if (m_offscreen) return m_offscreen->GetTexture();
  return GL_NONE;
}

void GameBase::SwapBuffer()
{
  if (m_offscreen) m_offscreen->SwapTexture();

  if (m_config.use_swap) {
    SDL_GL_SwapWindow(m_window);
  }
}

GameBaseConfig::GameBaseConfig(): use_swap(false), fps_time(16) {}

int GameBase::Run()
{
  std::cout << "Game thread start" << std::endl;
  SDL_GL_MakeCurrent(m_window, m_glcontext);

  CreateFramebuffer();

  CompileShader();
  ConfigureVertex();

  if (m_offscreen) m_offscreen->Activate();

  glViewport(0, 0, m_screen_width, m_screen_height);

  Uint64 old_ticks = SDL_GetTicks64();
  Uint64 last_tick = old_ticks;

  int frame_count = 0;
  while (!m_request_stop) {
    Render();
    SwapBuffer();

    Uint64 current_tick = SDL_GetTicks64();

    if (!m_config.use_swap && m_config.fps_time > 0) {
      int wait = m_config.fps_time;
      Uint64 next_tick = last_tick + wait;
      if (current_tick <= next_tick) {
        wait = next_tick - current_tick;
      } else {
        wait = 0;
        next_tick = current_tick;
      }
      if (wait > 0) SDL_Delay(wait);
      last_tick = next_tick;
    }

    frame_count += 1;
    if (current_tick > old_ticks + 1000) {
      m_framerate = frame_count * 1000.0f / (float)(current_tick - old_ticks);
      frame_count = 0;
      old_ticks = current_tick;
    }
  }
  return 0;
}

void GameBase::StartThread() {
  if (m_thread) return;
  m_thread = new std::thread([&]{
    Run();
    m_stopped = true;
  });
}

void GameBase::WaitToStop() {
  m_request_stop = true;
  if (m_thread) {
    m_thread->join();
  }
}

int GameBase::ID = 0;
