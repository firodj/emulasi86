#include <glad/gl.h>
#include "emulasi/gfx/offscreen.hpp"
#include <iostream>
#include "kosongg/glutil.hpp"

namespace emulasi {

void Offscreen::Init(int screenWidth, int screenHeight) {
	glGenFramebuffers(1, &m_framebuf);
	glBindFramebuffer(GL_FRAMEBUFFER, m_framebuf);

	glGenTextures(2, m_tex);

	glBindTexture(GL_TEXTURE_2D, m_tex[0]);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, screenWidth, screenHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glBindTexture(GL_TEXTURE_2D, m_tex[1]);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, screenWidth, screenHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	int tex_offscreen = (m_texFlipFlop + 1) % 2;
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_tex[tex_offscreen], 0);

	glGenRenderbuffers(1, &m_renderbuf);
	glBindRenderbuffer(GL_RENDERBUFFER, m_renderbuf);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, screenWidth, screenHeight);

	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, m_renderbuf);
	kosongg::CheckGLError(__FILE__, __LINE__);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
		std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	m_width = screenWidth;
	m_height = screenHeight;
	m_isOk = true;
}

unsigned int Offscreen::GetTexture() {
	std::unique_lock<std::mutex> lck(m_mtxRenderFull);
	return m_tex[ m_texFlipFlop ];
}

void Offscreen::SwapTexture() {
	std::unique_lock<std::mutex> lck(m_mtxRenderFull);

	glFlush();

	m_texFlipFlop = (m_texFlipFlop + 1) % 2;
	int tex_offscreen = (m_texFlipFlop + 1) % 2;

	glBindFramebuffer(GL_FRAMEBUFFER, m_framebuf);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_tex[tex_offscreen], 0);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
}

void Offscreen::Activate() {
	glBindFramebuffer(GL_FRAMEBUFFER, m_framebuf);
	kosongg::CheckGLError(__FILE__, __LINE__);
	glViewport(0, 0, m_width, m_height);
}

bool Offscreen::IsOk() { return m_isOk; }

