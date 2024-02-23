#pragma once

typedef unsigned int GLuint;

class PlainBox {
public:
  PlainBox();
  void Init();
  void Draw();

protected:
  GLuint m_vao;
};
