#include "emulasi/gfx/dummybox.hpp"
#include <glad/gl.h>

namespace emulasi
{
void DummyBox::Init() {
	glGenVertexArrays(1, &m_vao);
	glBindVertexArray(m_vao);

	glDisable(GL_CULL_FACE);
	glCullFace(GL_FRONT);
}

void DummyBox::Draw() {
	glBindVertexArray(m_vao);
}

}
