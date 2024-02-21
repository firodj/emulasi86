// Game.cpp

#include "GameBase.h"
#include <SDL.h>
#include <iostream>
#include "../kosongg/vendor/Engine.h"
#include "../kosongg/vendor/GLUtil.h"

#include <chrono>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

static const char *vertexShaderSource = "#version 330 core\n"
    "layout (location = 0) in vec3 aPos;\n"
    "uniform mat4 projection;\n"
    "uniform mat4 view;\n"
    "uniform mat4 model;\n"
    "void main()\n"
    "{\n"
    "  gl_Position = projection * view * model * vec4(aPos, 1.0f);\n"
    "}";

static const char *fragmentShaderSource = "#version 330 core\n"
    "out vec4 FragColor;\n"
    "void main()\n"
    "{\n"
    "   FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);\n"
    "}";

GameBase::GameBase(GameBaseConfig config): m_shader_program(-1), m_fb(-1), m_rb(-1), m_tex_flip_flop(0),
  m_request_stop(false), m_thread(nullptr), m_framerate(0.0)
{
  m_tex[0] = -1;
  m_tex[1] = -1;
  m_config = config;
  std::cout << "Game constructed" << std::endl;
}

GameBase::~GameBase()
{
  if (m_thread) delete m_thread;
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

  SDL_GLContext oldctx = SDL_GL_GetCurrentContext();
  if (oldctx == nullptr) {
    printf("Error: SDL_GL_GetCurrentContext(): %s\n", SDL_GetError());
  }

  m_glcontext = SDL_GL_CreateContext(m_window);
  if (m_glcontext == nullptr) {
      printf("Error: SDL_GL_CreateContext(): %s\n", SDL_GetError());
      return;
  }

  SDL_GL_GetDrawableSize(m_window, &m_screen_width, &m_screen_height);
  m_hidpi_x = (float)m_screen_width / m_window_width;
  m_hidpi_y = (float)m_screen_height / m_window_height;

  if (m_config.use_swap) {
    if (SDL_GL_SetSwapInterval(1)) { // Enable vsync
      printf("Error: SDL_GL_SetSwapInterval(): %s\n", SDL_GetError());
    }
  }

  SDL_GL_MakeCurrent(m_window, oldctx);
}

void GameBase::Finish()
{
    SDL_GL_DeleteContext(m_glcontext);
    SDL_DestroyWindow(m_window);
}

void GameBase::CreateFramebuffer()
{
  // framebuffer configuration
  glGenFramebuffers(1, &m_fb);
  glBindFramebuffer(GL_FRAMEBUFFER, m_fb);

  // create a color attachment texture
  glGenTextures(2, m_tex);

  glBindTexture(GL_TEXTURE_2D, m_tex[0]);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, m_screen_width, m_screen_height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  glBindTexture(GL_TEXTURE_2D, m_tex[1]);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, m_screen_width, m_screen_height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  int tex_offscreen = (m_tex_flip_flop + 1) % 2;
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_tex[tex_offscreen], 0);

  // create a renderbuffer object for depth and stencil attachment (we won't be sampling these)
  glGenRenderbuffers(1, &m_rb);
  glBindRenderbuffer(GL_RENDERBUFFER, m_rb);
  glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, m_screen_width, m_screen_height);

  // use a single renderbuffer object for both a depth AND stencil buffer.
  glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, m_rb);
  kosongg::CheckGLError(__FILE__, __LINE__);
  // now actually attach it

  if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;

  glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void GameBase::CompileShader()
{
  // build and compile our shader program
  // ------------------------------------
  // vertex shader
  int vertexShader = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
  glCompileShader(vertexShader);

  // check for shader compile errors
  int success;
  char infoLog[512];
  glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
  if (!success)
  {
    glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
    std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
  }

  // fragment shader
  int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
  glCompileShader(fragmentShader);
  // check for shader compile errors
  glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
  if (!success)
  {
    glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
    std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
  }

  // link shaders
  int shaderProgram = glCreateProgram();
  glAttachShader(shaderProgram, vertexShader);
  glAttachShader(shaderProgram, fragmentShader);
  glLinkProgram(shaderProgram);
  // check for linking errors
  glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
  if (!success) {
    glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
    std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
  }
  glDeleteShader(vertexShader);
  glDeleteShader(fragmentShader);

  m_shader_program = shaderProgram;
}

void GameBase::ConfigureVertex()
{
  // set up vertex data (and buffer(s)) and configure vertex attributes
  // ------------------------------------------------------------------
  float vertices[] = {
       0.5f,  0.5f, -0.0f,  // top right
       0.5f, -0.5f, -0.0f,  // bottom right
      -0.5f, -0.5f, -0.0f,  // bottom left
      -0.5f,  0.5f, -0.0f   // top left
  };
  unsigned int indices[] = {  // note that we start from 0!
      0, 1, 3,  // first Triangle
      1, 2, 3   // second Triangle
  };
  unsigned int VBO, VAO, EBO;
  glGenVertexArrays(1, &VAO);
  glGenBuffers(1, &VBO);
  glGenBuffers(1, &EBO);
  // bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
  glBindVertexArray(VAO);
  m_vao = VAO;

  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
  glEnableVertexAttribArray(0);

  // note that this is allowed, the call to glVertexAttribPointer registered VBO as the vertex attribute's bound vertex buffer object so afterwards we can safely unbind
  //glBindBuffer(GL_ARRAY_BUFFER, 0);

  // remember: do NOT unbind the EBO while a VAO is active as the bound element buffer object IS stored in the VAO; keep the EBO bound.
  //glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

  // You can unbind the VAO afterwards so other VAO calls won't accidentally modify this VAO, but this rarely happens. Modifying other
  // VAOs requires a call to glBindVertexArray anyways so we generally don't unbind VAOs (nor VBOs) when it's not directly necessary.

  glBindVertexArray(0);
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

  glUseProgram(m_shader_program);

  glUniformMatrix4fv(glGetUniformLocation(m_shader_program, "projection"), 1, GL_FALSE, glm::value_ptr(proj));
  glUniformMatrix4fv(glGetUniformLocation(m_shader_program, "view"), 1, GL_FALSE, glm::value_ptr(view));
  glUniformMatrix4fv(glGetUniformLocation(m_shader_program, "model"), 1, GL_FALSE, glm::value_ptr(model));

  glBindVertexArray(m_vao); // seeing as we only have a single VAO there's no need to bind it every time, but we'll do so to keep things a bit more organized
  //glDrawArrays(GL_TRIANGLES, 0, 6);
  glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
  // glBindVertexArray(0); // no need to unbind it every time
}

GLuint GameBase::tex() {
  std::unique_lock<std::mutex> lck(m_mtx_render_full);
  return m_tex[ m_tex_flip_flop ];
}

void GameBase::SwapBuffer()
{
  std::unique_lock<std::mutex> lck(m_mtx_render_full);

  glFlush();

  m_tex_flip_flop = (m_tex_flip_flop + 1) % 2;
  int tex_offscreen = (m_tex_flip_flop + 1) % 2;

  glBindFramebuffer(GL_FRAMEBUFFER, m_fb);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_tex[tex_offscreen], 0);

  if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;

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

  glBindFramebuffer(GL_FRAMEBUFFER, m_fb);
  kosongg::CheckGLError(__FILE__, __LINE__);

  glViewport(0, 0, m_screen_width, m_screen_height);

  Uint64 old_ticks = SDL_GetTicks64();
  Uint64 last_tick = old_ticks;

  int frame_count = 0;
  while (!m_request_stop) {
    SDL_Event event;
    while (SDL_PollEvent(&event)) ;

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
    });
}

void GameBase::WaitToStop() {
    m_request_stop = true;
    if (m_thread) {
        m_thread->join();
    }
}
