#pragma once

#include <emulasi/core/gameemu_interface.hpp>

namespace emulasi {

class GameEmuGfx: public IGameEmuGfx {
public:
	GameEmuGfx();

	void StretchBlt(int32_t texDest, int32_t wDest, int32_t hDest, void *data) override;
};

};
