#pragma once

#include <mutex>

namespace emulasi
{
class Offscreen
{
public:
	Offscreen() {};

	void Init(int screenWidth, int screenHeight);
	unsigned int GetTexture();
	void SwapTexture();
	void Activate();
	bool IsOk();

protected:
	unsigned int m_framebuf{}, m_renderbuf{}, m_tex[2]{};
	int m_texFlipFlop{}, m_width{}, m_height{};
	std::mutex m_mtxRenderFull{};
	bool m_isOk{};
};

};
