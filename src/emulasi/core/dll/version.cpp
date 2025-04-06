#include "version.hpp"
#include "emulasi/core/ms_windows.h"
#include "emulasi/core/export.hpp"
#include "emulasi/core/emulation.hpp"
#include "emulasi/core/timer.hpp"

#include <fmt/core.h>
#include <fmt/color.h>

#include "emulasi/util/log.hpp"
#include "emulasi/ui/gameemu.hpp"

namespace emulasi
{
////////////////////////////////////////////////////////////////////////////////

/**
 * GetFileVersionInfoSizeA
 * @return DWORD
 * @note CD check
 * @remark ms-win-core.dll / Mincore.lib / Version.lib
 */
ExportReturnParam API__GetFileVersionInfoSizeA(GameEmuInterface *game_,
	char *lptstrFilename, // [in] LPCSTR
	uint32_t *lpdwHandle // [out, optional] LPDWORD
)
{
	fmt::print(stderr, "{}(lptstrFilename={},lpdwHandle={})\n",
		fmt::styled("GetFileVersionInfoSizeA", fmt::fg(fmt::color::gold)),
		fmt::ptr(lptstrFilename),
		fmt::ptr(lpdwHandle));

	if (lptstrFilename) {
		fmt::print(stderr, "-- strFilename = {}\n",
			fmt::styled(lptstrFilename, fmt::fg(fmt::color::lime))
		);
	}

	if (lpdwHandle) {
		*lpdwHandle = 0;
	}

	return 0;
}

////////////////////////////////////////////////////////////////////////////////

VersionDll::VersionDll(GameEmuInterface* game): DllBase(game) {}
VersionDll::~VersionDll() {}
void VersionDll::Register() {
	game_->ExportAddT<API__GetFileVersionInfoSizeA>("GetFileVersionInfoSizeA");
}

////////////////////////////////////////////////////////////////////////////////
}
