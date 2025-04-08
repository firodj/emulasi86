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

static API__MSG pendingMsg = {0};

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

/**
 * MessageBoxA
 * @return int
 */
ExportReturnParam API__MessageBoxA(GameEmuInterface * game_,
	uint32_t hWnd,   // [in, optional] HWND
	Ptr32 lpText,    // [in, optional] LPCSTR
	Ptr32 lpCaption, // [in, optional] LPCSTR
	uint32_t uType)  // [in]           UINT
{

	fmt::print(stderr, "{}(hWnd={},lpText={},lpCaption={},uType={:#x})\n",
		fmt::styled("MessageBoxA", fmt::fg(fmt::color::gold)),
		fmt::styled(lpText, fmt::fg(fmt::color::aqua)),
		fmt::styled(lpCaption, fmt::fg(fmt::color::aqua)),
		fmt::styled(uType, fmt::fg(fmt::color::fuchsia))
	);

	// state:0
	// + Create Dialog Window
	// + Thread Focus on Dialog Window (DlgProc)

	fmt::print(stderr, "-- szText={}\n",
		fmt::styled(lpText.Mem<char>(game_), fmt::fg(fmt::color::lime)));
	fmt::print(stderr, "szCaption={}\n",
		fmt::styled(lpCaption.Mem<char>(game_), fmt::fg(fmt::color::lime)));

	// state: 1
	// + Waiting untuk One of BUtton Selected
	// + if answered, next state: 2
	// + other wise, Skip Thread

	// state: 2
	// SDL_Delay(5000);

	return 2;
}

int CheckMouse(GameEmuInterface * game_, API__MSG* lpMsg, uint32_t hWnd)
{
	bool *buttons = game_->mouseBtn();
	static bool prevButtons0 = false;
	int result =  0;

	if (!prevButtons0 && buttons[0]) {
		memset(lpMsg, 0x00, sizeof(API__MSG));

		lpMsg->message = API(WM_LBUTTONDOWN);
		lpMsg->hwnd = hWnd;
		result = 1;
		fmt::print("WM_LBUTTONDOWN {:#x}\n", lpMsg->hwnd);
	} else if (prevButtons0 && !buttons[0]) {
		memset(lpMsg, 0x00, sizeof(API__MSG));

		lpMsg->message = API(WM_LBUTTONUP);
		lpMsg->hwnd = hWnd;
		result = 1;
		fmt::print("WM_LBUTTONUP {:#x}\n", lpMsg->hwnd);
	}
	prevButtons0 = buttons[0];

	return result;
}

/**
 * PeekMessageA
 * @return BOOL
 */
ExportReturnParam API__PeekMessageA(GameEmuInterface * game_,
	Ptr32     ptrlpMsg,         // [out] LPMSG
	uint32_t  hWnd,          // [in, optional] HWND
	uint32_t  wMsgFilterMin, // [in] UINT
	uint32_t  wMsgFilterMax, // [in] UINT
	uint32_t  wRemoveMsg     // [in] UINT
)
{
#if 0
	fmt::print(stderr, "{}(lpMsg={},hWnd={},wMsgFilterMin={},wMsgFilterMax={},wRemoveMsg={})\n",
		fmt::styled("PeekMessageA", fmt::fg(fmt::color::gold)),
		fmt::styled(ptrlpMsg, fmt::fg(fmt::color::aqua)),
		fmt::styled(hWnd, fmt::fg(fmt::color::fuchsia)),
		fmt::styled(wMsgFilterMin, fmt::fg(fmt::color::fuchsia)),
		fmt::styled(wMsgFilterMax, fmt::fg(fmt::color::fuchsia)),
		fmt::styled(wRemoveMsg, fmt::fg(fmt::color::fuchsia))
	);
#endif

	API__MSG *lpMsg = ptrlpMsg.Mem<API__MSG>(game_);

	int result = 0;

	const bool SHOULD_SDL_CLOSE = false;
	if (SHOULD_SDL_CLOSE) {
		memset(lpMsg, 0x00, sizeof(API__MSG));

		lpMsg->message = API__WM_DESTROY;
		lpMsg->hwnd = hWnd;

		pendingMsg = *lpMsg;
		result = 1;
	} else {
		result = CheckMouse(game_, lpMsg, hWnd);
		if (result) {
			pendingMsg = *lpMsg;
		}
	}

	if (wRemoveMsg == API__PM_REMOVE) {
		memset(&pendingMsg, 0x00, sizeof(API__MSG));
	}

	return result;
}

/**
 * GetMessageA
 * @return BOOL
 */
ExportReturnParam API__GetMessageA(GameEmuInterface *game_,
	Ptr32    ptrlpMsg,         // [out] LPMSG
	uint32_t hWnd,          // [in, optional] HWND
	uint32_t wMsgFilterMin, // [in] UINT
	uint32_t wMsgFilterMax  // [in] UINT
)
{
	fmt::print(stderr, "{}(lpMsg={},hWnd={},wMsgFilterMin={},wMsgFilterMax={})\n",
		fmt::styled("GetMessageA", fmt::fg(fmt::color::gold)),
		fmt::styled(ptrlpMsg, fmt::fg(fmt::color::aqua)),
		fmt::styled(hWnd, fmt::fg(fmt::color::fuchsia)),
		fmt::styled(wMsgFilterMin, fmt::fg(fmt::color::fuchsia)),
		fmt::styled(wMsgFilterMax, fmt::fg(fmt::color::fuchsia))
	);

	API__MSG *lpMsg = ptrlpMsg.Mem<API__MSG>(game_);

	int result = 1;
	const bool SHOULD_SDL_CLOSE = false;

	if (pendingMsg.message) {
		*lpMsg = pendingMsg;

		memset(&pendingMsg, 0x00, sizeof(API__MSG));
	} else {
		if (SHOULD_SDL_CLOSE) {
			memset(lpMsg, 0x00, sizeof(API__MSG));

			lpMsg->message = API(WM_DESTROY);
			lpMsg->hwnd = hWnd;
		} else {
			result = CheckMouse(game_, lpMsg, hWnd);
		}
	}

	result = (lpMsg->message == API(WM_QUIT) || lpMsg->message == API(WM_DESTROY)) ? 0 : 1;

	return result;
}

/**
 * TranslateMessage
 * @return BOOL
 */
ExportReturnParam API__TranslateMessage(GameEmuInterface * game_,
	Ptr32 ptrlpMsg // [in] const MSG
)
{
	fmt::print(stderr, "{}(lpMsg={})\n",
		fmt::styled("TranslateMessage", fmt::fg(fmt::color::gold)),
		fmt::styled(ptrlpMsg, fmt::fg(fmt::color::aqua))
	);

	API__MSG *lpMsg = ptrlpMsg.Mem<API__MSG>(game_);

	return 0;
}

/**
 * DefWindowProcA
 * @return LRESULT
 */
ExportReturnParam API__DefWindowProcA(GameEmuInterface *game_,
	uint32_t hWnd, // [in] HWND
	uint32_t Msg, // [in] UINT
	uint32_t wParam, // [in] WPARAM
	uint32_t lParam) // [in] LPARAM
{
	fmt::print(stderr, "{}(hWnd={},Msg={},wParam={},lParam={})\n",
		fmt::styled("DefWindowProcA", fmt::fg(fmt::color::gold)),
		fmt::styled(hWnd, fmt::fg(fmt::color::fuchsia)),
		fmt::styled(Msg, fmt::fg(fmt::color::fuchsia)),
		fmt::styled(wParam, fmt::fg(fmt::color::fuchsia)),
		fmt::styled(lParam, fmt::fg(fmt::color::fuchsia))
	);

	return 0;
}

/**
 * MapVirtualKeyA
 * @return UINT
 */
ExportReturnParam API__MapVirtualKeyA(GameEmuInterface * game_,
	uint32_t uCode, // [in] UINT
	uint32_t uMapType // [in] UINT
)
{
	uint32_t returnValue = 0;

	switch(uMapType) {
		case 1:
			if (uCode == API(VK_LSHIFT) || uCode == API(VK_RSHIFT)) {
				returnValue = API(VK_SHIFT);
			} else if (uCode == API(VK_LCONTROL) || uCode == API(VK_RCONTROL)) {
				returnValue = API(VK_CONTROL);
			} else if (uCode == API(VK_LMENU) || uCode == API(VK_RMENU)) {
				returnValue = API(VK_MENU);
			} else {
				returnValue = uCode;
			}
			break;
		case 2:
			if (uCode >= 0x41 && uCode <= 0x5A) {
				returnValue = 'a' + uCode - 0x41;
			}
			break;
		default:
			fmt::print(stderr, fmt::fg(fmt::color::crimson), "unknown key map mode {} in MapVirtualKeyA\n", uMapType);
			game_->PanicEmulation();
			break;
	}

	return returnValue;
}

/**
 * GetKeyState
 * @return SHORT
 */
ExportReturnParam API__GetKeyState(GameEmuInterface *game_,
	int32_t nVirtKey // [in] int
)
{
	fmt::print(stderr, "{}(nVirtKey={:#x})\n",
		fmt::styled("GetKeyState", fmt::fg(fmt::color::gold)),
		fmt::styled(nVirtKey, fmt::fg(fmt::color::fuchsia))
	);

	int pressed = 0x8000; // high order bit = pressed
	int toggled = 0x0001; // low order bit = toggled
	int returnValue = 0;  // default: unpressed

	switch(nVirtKey) {
		case API__VK_CAPITAL:
			returnValue = 0;
			break;
		default:
			break;
	}

	return returnValue;
}

const char* IconName(GameEmuInterface * game_, Ptr32 ptr)
{
	if ((ptr.val & 0xFFFF0000) != 0) {
		return ptr.Mem<char>(game_);
	}

	uint16_t resource_id = ptr.val & 0xFFFF;
	const char* s;

	switch(resource_id) {
	case API__IDI_APPLICATION:
		s = "<IDI_APPLICATION>";
		break;
	default:
		fmt::print(stderr, "Unknown icon-name {:#x}\n", resource_id);
		s = "<unknown>";
		game_->PanicEmulation();
		break;
	}

	return s;
}

/**
 * LoadIconA
 * @return HICON
 */
ExportReturnParam API__LoadIconA(GameEmuInterface *game_,
	uint32_t hInstance,    // [in, optional] HINSTANCE
	Ptr32    ptrlpIconName // [in] LPCSTR
)
{
	fmt::print(stderr, "{}(hInstance={:#x},lpIconName={})\n",
		fmt::styled("LoadIconA",   fmt::fg(fmt::color::gold)),
		fmt::styled(hInstance,     fmt::fg(fmt::color::fuchsia)),
		fmt::styled(ptrlpIconName, fmt::fg(fmt::color::fuchsia))
	);

	const char *lpIconName = IconName(game_, ptrlpIconName);

	fmt::print(stderr, "-- lpIconName={}\n",
		fmt::styled(lpIconName, fmt::fg(fmt::color::lime)));

	return 0;
}

/**
 * LoadCursorA
 * @return HCURSOR
 */
ExportReturnParam API__LoadCursorA(GameEmuInterface *game_,
	uint32_t hInstance,      // [in, optional] HINSTANCE
	Ptr32    ptrlpCursorName // [in] LPCSTR
)
{
	fmt::print(stderr, "{}(hInstance={:#x},lpCursorName={})\n",
		fmt::styled("LoadCursorA", fmt::fg(fmt::color::gold)),
		fmt::styled(hInstance, fmt::fg(fmt::color::fuchsia)),
		fmt::styled(ptrlpCursorName, fmt::fg(fmt::color::fuchsia))
	);

	const char *lpCursorName = IconName(game_, ptrlpCursorName);

	fmt::print(stderr, "-- lpCursorName={}\n",
		fmt::styled(lpCursorName, fmt::fg(fmt::color::lime)));

	return 0;
}

/**
 * GetSystemMetrics
 * @return int
 */
ExportReturnParam API__GetSystemMetrics(GameEmuInterface *game_,
	int32_t nIndex // [in] int
)
{
	int returnValue = 0;

	fmt::print(stderr, "{}(...)\n", fmt::styled("GetSystemMetrics", fmt::fg(fmt::color::gold)));

	switch(nIndex) {
		case 0:
			returnValue = game_ ? game_->windowWidth() : 640; // Horizontal resolution
			break;
		case 1:
			returnValue = game_ ? game_->windowHeight() : 480; // Vertical resolution
			break;
		case 15:
			returnValue = 0; //FIXME
			break;
		case 32:
			returnValue = 0; //FIXME
			break;
		default:
			returnValue = 16;
			fmt::print(stderr, fmt::fg(fmt::color::crimson), "Unknown metric\n");
			game_->PanicEmulation();
			break;
	}

	return returnValue;
}

/**
 * RegisterClassA
 * @return ATOM
 */
ExportReturnParam API__RegisterClassA(GameEmuInterface * game_,
	Ptr32 ptrlpWndClass // [in] const WNDCLASSA *
)
{
	int returnValue = 0;

	fmt::print(stderr, "{}(...)\n", fmt::styled("RegisterClassA", fmt::fg(fmt::color::gold)));

	API__WNDCLASSA* lpWndClass = ptrlpWndClass.Mem<API__WNDCLASSA>(game_);
	fmt::print(stderr, "-- .lpfnWndProc: {:#x}\n", lpWndClass->lpfnWndProc );

	if ((lpWndClass->lpszClassName & 0xFFFF000000)) {
		char *lpszClassName = (char*)game_->Memory(lpWndClass->lpszClassName);
		fmt::print(stderr, "-- .lpszClassName: {:s}\n", lpszClassName );
	}

	returnValue = 444;

	return returnValue;
}

/**
 * RegisterClassExA
 * @return ATOM
 */
ExportReturnParam API__RegisterClassExA(GameEmuInterface * game_,
	Ptr32 ptrlpWndClassEx // [in] const WNDCLASSEXA *
)
{
	int returnValue = 0;

	fmt::print(stderr, "{}(...)\n", fmt::styled("RegisterClassExA", fmt::fg(fmt::color::gold)));

	API__WNDCLASSEXA* lpWndClassEx = ptrlpWndClassEx.Mem<API__WNDCLASSEXA>(game_);
	fmt::print(stderr, "-- .lpfnWndProc: {:#x}\n", lpWndClassEx->lpfnWndProc );

	if ((lpWndClassEx->lpszClassName & 0xFFFF00000)) {
		char *lpszClassName = (char*)game_->Memory(lpWndClassEx->lpszClassName);
		fmt::print(stderr, "-- .lpszClassName: {:s}\n", lpszClassName);
		*(uint32_t*)game_->Memory(game_->wndMainProc()) = lpWndClassEx->lpfnWndProc;
	}

	returnValue = 444;

	return returnValue;
}

/**
 * CreateWindowExA
 * @return HWND
 */
ExportReturnParam API__CreateWindowExA(GameEmuInterface * game_,
	uint32_t dwExStyle,    // [in] DWORD
	Ptr32    ptrlpClassName,  // [in, optional] LPCSTR
	Ptr32    ptrlpWindowName, // [in, optional] LPCSTR
	uint32_t dwStyle,      // [in] DWORD
	int32_t  X,            // [in] int
	int32_t  Y,            // [in] int
	int32_t  nWidth,       // [in] int
	int32_t  nHeight,      // [in] int
	uint32_t hWndParent,   // [in, optional] HWND
	uint32_t hMenu,        // [in, optional] HMENU
	uint32_t hInstance,    // [in, optional] HINSTANCE
	Ptr32    ptrlpParam    // [in, optional] LPVOID
)
{
	int returnValue = 0;

	char *lpClassName = nullptr;
	char *lpWindowName = nullptr;

	fmt::print(stderr, "{}(...)\n", fmt::styled("CreateWindowExA", fmt::fg(fmt::color::gold)));

	if (ptrlpClassName) {
		lpClassName = ptrlpClassName.Mem<char>(game_);
		fmt::print(stderr, "-- lpClassName: {:s}\n",  fmt::styled(lpClassName,  fmt::fg(fmt::color::lime)));
	}

	if (ptrlpWindowName) {
		lpWindowName = ptrlpWindowName.Mem<char>(game_);
		fmt::print(stderr, "-- lpWindowName: {:s}\n", fmt::styled(lpWindowName, fmt::fg(fmt::color::lime)));
	}

	returnValue = 333;

	return returnValue;
}

/**
 * SetWindowPos
 * @return BOOL
 */
ExportReturnParam API__SetWindowPos(GameEmuInterface * game_,
	uint32_t hWnd,            // [in] HWND
	uint32_t hWndInsertAfter, // [in, optional] HWND
	int32_t  X,               // [in] int
	int32_t  Y,               // [in] int
	int32_t  cx,              // [in] int
	int32_t  cy,              // [in] int
	uint32_t uFlag            // [in] UINT
)
{
	int returnValue = 0;

	fmt::print(stderr, "{}(...)\n", fmt::styled("SetWindowPos", fmt::fg(fmt::color::gold)));

	returnValue = 1;
	return returnValue;
}

/**
 * ShowWindow
 * @return BOOL
 */
ExportReturnParam API__ShowWindow(GameEmuInterface * game_,
	uint32_t hWnd,    // [in] HWND
	int32_t  nCmdShow // [in] int
)
{
	int returnValue = 0;

	fmt::print(stderr, "{}(...)\n", fmt::styled("ShowWindow", fmt::fg(fmt::color::gold)));

	returnValue = 0;

	return returnValue;
}

/**
 * UpdateWindow
 * @return BOOL
 */
ExportReturnParam API__UpdateWindow(GameEmuInterface * game_,
	uint32_t hWnd // [in] HWND
)
{
	int returnValue = 0;

	fmt::print(stderr, "{}(...)\n", fmt::styled("UpdateWindow", fmt::fg(fmt::color::gold)));

	returnValue = 1;

	return returnValue;
}

/**
 * SetCursor
 * @return HCURSOR
 */
ExportReturnParam API__SetCursor(GameEmuInterface * game_,
	uint32_t hCursor // [in, optional] HCURSOR
)
{
	int returnValue = 0;

	fmt::print(stderr, "{}(...)\n", fmt::styled("SetCursor", fmt::fg(fmt::color::gold)));

	returnValue = 0;

	return ExportReturnParam{returnValue, 1};
}

/**
 * wsprintfA
 * @return int WINAPIV (cdecl)
 */
ExportReturnParam User32Dll::wsprintfA(ExportStackParam* param)
{
	char* lpOut      = (char*)game_->Memory(param->args[0]);
	const char* lpIn = (char*)game_->Memory(param->args[1]);

	int count = 0;
	unsigned int stackIndex = 2;
	while(*lpIn != '\0') {
		const char* nextPercent = strchr(lpIn, '%');
		if (nextPercent == NULL) {
			count += sprintf(&lpOut[count], "%s", lpIn);
			break;
		}
		unsigned int length = nextPercent - lpIn;
		memcpy(&lpOut[count], lpIn, length);
		lpIn += length;
		count += length;
		lpIn = nextPercent + 1;
		char type = *lpIn++;
		switch(type) {
			case 'c':
				count += sprintf(&lpOut[count], "%c", param->args[stackIndex++]);
				break;
			case 's':
				count += sprintf(&lpOut[count], "%s", (char*)game_->Memory(param->args[stackIndex++]));
				break;
			case 'd':
				count += sprintf(&lpOut[count], "%d", param->args[stackIndex++]);
				break;
			default:
				fmt::print(stderr, fmt::fg(fmt::color::crimson), "Unknown format type '%c'\n", type);
				assert(false);
		}
	}

	fmt::print(stderr, "Out: '{:s}'\n", lpOut);
	return ExportReturnParam{
		count, 0,
	}; // __cdecl!
}

/**
 * DispatchMessageA
 * @return LRESULT
 */
ExportReturnParam API__DispatchMessageA(GameEmuInterface * game_,
	Ptr32 ptrlpMsg) // [in] const MSG
{
	int32_t state = game_->Reg(emulasi::EBP);
	int32_t esp = game_->Reg(emulasi::ESP);

	fmt::print(stderr, "{}(lpMsg={})<state={},esp={}>\n",
		fmt::styled("DispatchMessageA", fmt::fg(fmt::color::gold)),
		fmt::styled(ptrlpMsg, fmt::fg(fmt::color::aqua)),
		fmt::styled(state, fmt::fg(fmt::color::hot_pink)),
		fmt::styled(esp, fmt::fg(fmt::color::hot_pink))
	);

	switch (state) { case 0:
	{
		API__MSG *lpMsg = ptrlpMsg.Mem<API__MSG>(game_);

		fmt::print(stderr, "-- lpMsg=[lParam={:#x},", lpMsg->lParam);
		fmt::print("wParam={:#x},", lpMsg->wParam);
		fmt::print("message={:#x},", lpMsg->message);
		fmt::print("hwnd={:#x}]", lpMsg->hwnd);

		game_->StackPush(lpMsg->lParam);
		game_->StackPush(lpMsg->wParam);
		game_->StackPush(lpMsg->message);
		game_->StackPush(lpMsg->hwnd);

		Address cb = *(Address*)game_->Memory(game_->wndMainProc());
		return ExportReturnParam::AsCall(cb, 1);
	}

	case 1: {
		API__LRESULT retVal = game_->Reg(emulasi::EAX);

		fmt::print(stderr, "retVal={:#x} esp={:#x}\n", (unsigned)retVal, (unsigned)esp);
		return ExportReturnParam{(signed)retVal, 1};
	}
	default:
		fmt::print(stderr, fmt::fg(fmt::color::crimson), "invalid state {}, possible incorrect stack order\n", state);
		game_->PanicEmulation();
	}
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
	game_->ExportAddT<API__MessageBoxA>("MessageBoxA");
	game_->ExportAddT<API__PeekMessageA>("PeekMessageA");
	game_->ExportAddT<API__GetMessageA>("GetMessageA");
	game_->ExportAddT<API__TranslateMessage>("TranslateMessage");
	Export& export_sym = game_->ExportAddT<API__DispatchMessageA>("DispatchMessageA");
	export_sym.isCodeWithCallback = true;

	game_->ExportAddT<API__DefWindowProcA>("DefWindowProcA");
	game_->ExportAddT<API__MapVirtualKeyA>("MapVirtualKeyA");
	game_->ExportAddT<API__GetKeyState>("GetKeyState");
	game_->ExportAddT<API__LoadIconA>("LoadIconA");
	game_->ExportAddT<API__LoadCursorA>("LoadCursorA");

	game_->ExportAddT<API__GetSystemMetrics>("GetSystemMetrics");
	game_->ExportAddT<API__RegisterClassA>("RegisterClassA");
	game_->ExportAddT<API__RegisterClassExA>("RegisterClassExA");
	game_->ExportAddT<API__CreateWindowExA>("CreateWindowExA");

	game_->ExportAddT<API__SetWindowPos>("SetWindowPos");
	game_->ExportAddT<API__ShowWindow>("ShowWindow");
	game_->ExportAddT<API__UpdateWindow>("UpdateWindow");
	game_->ExportAddT<API__SetCursor>("SetCursor");

	game_->exportAddOld("wsprintfA", std::bind(&User32Dll::wsprintfA, this, std::placeholders::_1));
}
////////////////////////////////////////////////////////////////////////////////
}
