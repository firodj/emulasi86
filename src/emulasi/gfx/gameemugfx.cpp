#include "gameemugfx.hpp"

#include <glad/gl.h>
#include <fmt/core.h>
#include <fmt/color.h>

namespace emulasi
{
////////////////////////////////////////////////////////////////////////////////

GameEmuGfx::GameEmuGfx() {}

void GameEmuGfx::StretchBlt(int32_t texDest, int32_t wDest, int32_t hDest, void *data)
{
	if (!texDest) {
		glClearColor(1.0f,0.0f,1.0f,1.0f);
		glClear(GL_COLOR_BUFFER_BIT);
		fmt::print(stderr, fmt::fg(fmt::color::crimson), "ERROR: invalid destination!\n");
		return;
	}

	GLint previousTexture = 0;
	glGetIntegerv(GL_TEXTURE_BINDING_2D, &previousTexture);
	glBindTexture(GL_TEXTURE_2D, texDest);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, wDest, hDest, 0, GL_BGR, GL_UNSIGNED_BYTE, data);
	glBindTexture(GL_TEXTURE_2D, previousTexture);
}

////////////////////////////////////////////////////////////////////////////////
}
