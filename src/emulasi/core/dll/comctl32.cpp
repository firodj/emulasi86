#include "comctl32.hpp"
#include "emulasi/core/ms_windows.h"
#include <fmt/core.h>
#include <fmt/color.h>
#include <string>
#include <cassert>

#include "emulasi/util/log.hpp"
#include <emulasi/core/gameemu_interface.hpp>

#define DBG_PRINT 1
namespace emulasi
{
////////////////////////////////////////////////////////////////////////////////

ExportReturnParam API__InitCommonControls(GameEmuInterface * game_)
{
	// Comctl32.lib / Comctl32.dll
#if DBG_PRINT
	fmt::print(stderr, "{:s}(void)\n",
		fmt::styled("InitCommonControls", fmt::fg(fmt::color::gold))
	);
#endif
	return ExportReturnParam::AsVoid();
}

ComCtl32Dll::ComCtl32Dll(GameEmuInterface* game): DllBase(game) {}
ComCtl32Dll::~ComCtl32Dll() {}
void ComCtl32Dll::Register() {
	game_->ExportAddT<API__InitCommonControls>("InitCommonControls");
}

////////////////////////////////////////////////////////////////////////////////
}
