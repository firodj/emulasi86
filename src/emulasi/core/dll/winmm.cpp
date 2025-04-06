#include "winmm.hpp"
#include "emulasi/core/ms_windows.h"
#include "emulasi/core/export.hpp"
#include "emulasi/core/emulation.hpp"
#include "emulasi/core/timer.hpp"

#include "emulasi/util/log.hpp"
#include "emulasi/ui/gameemu.hpp"

namespace emulasi
{
////////////////////////////////////////////////////////////////////////////////

/**
 * timeGetTime()
 * @return DWORD
 * @remark Winmm.lib
 */
ExportReturnParam API__timeGetTime(GameEmuInterface * game_)
{
	int ticks = GetTicks();
	return ticks;;
}

////////////////////////////////////////////////////////////////////////////////
WinMmDll::WinMmDll(GameEmuInterface* game): DllBase(game) {}
WinMmDll::~WinMmDll() {}
void WinMmDll::Register() {
	game_->ExportAddT<API__timeGetTime>("timeGetTime");
}

////////////////////////////////////////////////////////////////////////////////
}
