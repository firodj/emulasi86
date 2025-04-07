#include "emulasi/core/ms_windows.h"
#include "emulasi/core/export.hpp"
#include "emulasi/core/emulation.hpp"

#include "emulasi/ui/gameemu.hpp"
#include "emulasi/core/export.hpp"
#include "user32.hpp"

#include <string.h>
#include <cassert>
#include <iostream>
#include <fmt/core.h>
#include <fmt/color.h>

#include "emulasi/util/log.hpp"

#define DBG_PRINT 1
#define DBG_PRINT_CURSOR 0

static API(MSG) pendingMsg = {0};

namespace emulasi
{
////////////////////////////////////////////////////////////////////////////////
/**
 * void PostQuitMessage(
 *   [in] int nExitCode
 * );
 */
ExportReturnParam API__PostQuitMessage(GameEmuInterface * game_, int nExitCode)
{
	// TODO: PostQuitMessage function posts a WM_QUIT
#if DBG_PRINT
	fmt::print(stderr, "{:s}(nExitCode={})\n",
		fmt::styled("PostQuitMessage", fmt::fg(fmt::color::gold)),
		fmt::styled(nExitCode, fmt::fg(fmt::color::fuchsia))
	);
#endif

	return ExportReturnParam::AsVoid();
}

/**
 * GetCursorPos
 * @return BOOL
 */
ExportReturnParam API__GetCursorPos(GameEmuInterface *game_,
	Ptr32 lpPoint) // [out] LPPOINT
{
#if DBG_PRINT_CURSOR
	fmt::print(stderr, "{}(lpPoint={})\n",
		fmt::styled("GetCursorPos", fmt::fg(fmt::color::gold)),
		fmt::styled(lpPoint, fmt::fg(fmt::color::aqua))
	);
#endif

	int32_t* points = lpPoint.Mem<int32_t>(game_);

	float* mousePos = game_->mousePos();

	points[0] = (int)mousePos[0];
	points[1] = (int)mousePos[1];

	return 1;
}

/**
 * SetCursorPos
 * @return BOOL
 */
ExportReturnParam API__SetCursorPos(GameEmuInterface *game_,
	int32_t x, // [in]
	int32_t y // [in]
)
{
#if DBG_PRINT_CURSOR
	fmt::print(stderr, "{}(x={}, y={})\n",
		fmt::styled("SetCursorPos", fmt::fg(fmt::color::gold)),
		fmt::styled(x, fmt::fg(fmt::color::fuchsia)),
		fmt::styled(y, fmt::fg(fmt::color::fuchsia))
	);
#endif

	return 1;
}

/**
 * GetKeyNameTextA
 * @return int
 */
ExportReturnParam API__GetKeyNameTextA(GameEmuInterface *game_,
	int32_t lParam,  // [in]  LONG
	Ptr32 lpString,  // [out] LPSTR
	int32_t cchSize  // [in]  int
)
{
#if DBG_PRINT
	fmt::print(stderr, "{}(lParam={:#010x}, lpString={}, cchSize={})\n",
		fmt::styled("GetKeyNameTextA", fmt::fg(fmt::color::gold)),
		fmt::styled(lParam, fmt::fg(fmt::color::fuchsia)),
		fmt::styled(lpString, fmt::fg(fmt::color::aqua)),
		fmt::styled(cchSize, fmt::fg(fmt::color::fuchsia))
	);
#endif
	char* szString  = lpString.Mem<char>(game_);
	int code = (lParam >> 16) & 0x1FF;

	//int rpt  = lParam & 0xFFFF;
	//int sc   = (lParam >> 16) & 0xFF;
	//int ext  = (lParam >> 24) & 1;
	//int dont = (lParam >> 25) & 1;

	static std::map<int, const char *> keyTables = {
		{0x01, "Esc"},
		{0x0e, "Backspace"},
		{0x0f, "Tab"},
		{0x1c, "Enter"},
		{0x1d, "Ctrl"},
		{0x2a, "Shift"},
		{0x36, "Right Shift"},
		{0x37, "Num *"},
		{0x38, "Alt"},
		{0x39, "Space"},
		{0x3a, "Caps Lock"},
		{0x3b, "F1"},
		{0x3c, "F2"},
		{0x3d, "F3"},
		{0x3e, "F4"},
		{0x3f, "F5"},
		{0x40, "F6"},
		{0x41, "F7"},
		{0x42, "F8"},
		{0x43, "F9"},
		{0x44, "F10"},
		{0x45, "Pause"},
		{0x46, "Scroll Lock"},
		{0x47, "Num 7"},
		{0x48, "Num 8"},
		{0x49, "Num 9"},
		{0x4a, "Num -"},
		{0x4b, "Num 4"},
		{0x4c, "Num 5"},
		{0x4d, "Num 6"},
		{0x4e, "Num +"},
		{0x4f, "Num 1"},
		{0x50, "Num 2"},
		{0x51, "Num 3"},
		{0x52, "Num 0"},
		{0x53, "Num Del"},
		{0x54, "Sys Req"},
		{0x57, "F11"},
		{0x58, "F12"},
		{0x7c, "F13"},
		{0x7d, "F14"},
		{0x7e, "F15"},
		{0x7f, "F16"},
		{0x80, "F17"},
		{0x81, "F18"},
		{0x82, "F19"},
		{0x83, "F20"},
		{0x84, "F21"},
		{0x85, "F22"},
		{0x86, "F23"},
		{0x87, "F24"},
		{0x11c, "Num Enter"},
		{0x11d, "Right Ctrl"},
		{0x135, "Num /"},
		{0x137, "Prnt Ccrn"},
		{0x138, "Right Alt"},
		{0x145, "Num Lock"},
		{0x146, "Break"},
		{0x147, "Home"},
		{0x148, "Up"},
		{0x149, "Page Up"},
		{0x14b, "Left"},
		{0x14d, "Right"},
		{0x14f, "End"},
		{0x150, "Down"},
		{0x151, "Page Down"},
		{0x152, "Insert"},
		{0x153, "Delete"},
		{0x154, "<00>"},
		{0x156, "Help"},
		{0x15b, "Left Windows"},
		{0x15c, "Right Windows"},
		{0x15d, "Application"},
	};

	int returnValue = 0;
	if (keyTables.find(code) != keyTables.end()) {
		returnValue = snprintf(szString, cchSize, "%s", keyTables[code]);

#if DBG_PRINT
		fmt::print(stderr, "-- szString={}\n",
			fmt::styled(szString, fmt::fg(fmt::color::lime)));
#endif
	}
	return returnValue;
}

/**
 * FindWindowA
 * @return HWND
 */
ExportReturnParam API__FindWindowA(GameEmuInterface *game_,
	Ptr32 lpClassName, // [in, optional] LPCSTR
	Ptr32 lpWindowName) // [in, optional] LPCSTR
{
#if DBG_PRINT
	fmt::print(stderr, "{}(lpClassName={}, lpWindowName={})\n",
		fmt::styled("FindWindowA", fmt::fg(fmt::color::gold)),
		fmt::styled(lpClassName, fmt::fg(fmt::color::aqua)),
		fmt::styled(lpWindowName, fmt::fg(fmt::color::aqua))
	);
#endif

#if DBG_PRINT
	if (lpClassName)
	fmt::print(stderr, "-- szClassName = {}",
		fmt::styled(lpClassName.Mem<char>(game_), fmt::fg(fmt::color::lime)));

	if (lpWindowName)
	fmt::print(stderr, "-- szWindowName = {}",
		fmt::styled(lpWindowName.Mem<char>(game_), fmt::fg(fmt::color::lime)));
#endif

	return 0;
}

////////////////////////////////////////////////////////////////////////////////
User32Dll::User32Dll(GameEmuInterface* game): DllBase(game) {}
User32Dll::~User32Dll() {}

void User32Dll::Register()
{
	game_->ExportAddT<API__PostQuitMessage>("PostQuitMessage");
	game_->ExportAddT<API__GetCursorPos>("GetCursorPos");
	game_->ExportAddT<API__SetCursorPos>("SetCursorPos");
	game_->ExportAddT<API__GetKeyNameTextA>("GetKeyNameTextA");
	game_->ExportAddT<API__FindWindowA>("FindWindowA");

}
////////////////////////////////////////////////////////////////////////////////
}
