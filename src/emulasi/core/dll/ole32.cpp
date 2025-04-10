#include "ole32.hpp"
#include <emulasi/core/ms_windows.h>
#include <emulasi/core/export.hpp>

#include <fmt/core.h>
#include <fmt/color.h>
#include <string.h>
#include <libassert/assert.hpp>

#include <emulasi/util/log.hpp>
#include <emulasi/ui/gameemu.hpp>
#include <emulasi/core/guiddef.hpp>

#define DBG_PRINT 1

namespace emulasi
{
////////////////////////////////////////////////////////////////////////////////

/**
 * CoUninitialize
 * @remark Ole32.lib
 */
ExportReturnParam API__CoUninitialize(GameEmuInterface * game_)
{
#if DBG_PRINT
	fmt::print(stderr, "{:s}(void)\n",
		fmt::styled("CoUninitialize", fmt::fg(fmt::color::gold))
	);
#endif
	return ExportReturnParam::AsVoid();
}

/**
 * CoInitialize
 * @return HRESULT
 * @remark Ole32.lib
 */
ExportReturnParam API__CoInitialize(GameEmuInterface * game_,
	API__LPVOID pvReserved  // [in, optional] LPVOID
)
{
#if DBG_PRINT
	fmt::print(stderr, "{:s}(pvReserved={:#x})\n",
		fmt::styled("CoInitialize", fmt::fg(fmt::color::gold)),
		pvReserved
	);
#endif
	DEBUG_ASSERT(pvReserved == 0x00000000);
	return API__S_OK;
}

/**
 * CoCreateInstance
 * @return HRESULT
 * @remark Ole32.lib
 */
ExportReturnParam API__CoCreateInstance(GameEmuInterface * game_,
	API__CLSID * clsid, // [in] REFCLSID
	uint32_t pUnkOuter, // [in] LPUNKNOWN
	uint32_t dwClsContext, // [in] DWORD
	API__IID * iid, // [in] REFIID
	uint32_t* ppv // [out] LPVOID
)
{
	int returnValue = 0;

	char clsidString[1025];
	snprintf(clsidString, 1024, "%08" PRIX32 "-%04" PRIX16 "-%04" PRIX16 "-%02" PRIX8 "%02" PRIX8 "-%02" PRIX8 "%02" PRIX8 "%02" PRIX8 "%02" PRIX8 "%02" PRIX8 "%02" PRIX8,
					clsid->Data1, clsid->Data2, clsid->Data3,
					clsid->Data4[0], clsid->Data4[1], clsid->Data4[2], clsid->Data4[3],
					clsid->Data4[4], clsid->Data4[5], clsid->Data4[6], clsid->Data4[7]);
	fmt::print(stderr, "  (read clsid: {:s})\n", clsidString);

	char iidString[1025];
	snprintf(iidString, 1024, "%08" PRIX32 "-%04" PRIX16 "-%04" PRIX16 "-%02" PRIX8 "%02" PRIX8 "-%02" PRIX8 "%02" PRIX8 "%02" PRIX8 "%02" PRIX8 "%02" PRIX8 "%02" PRIX8,
					iid->Data1, iid->Data2, iid->Data3,
					iid->Data4[0], iid->Data4[1], iid->Data4[2], iid->Data4[3],
					iid->Data4[4], iid->Data4[5], iid->Data4[6], iid->Data4[7]);
	fmt::print(stderr, "  (read iid: {:s})\n", iidString);

	std::string name;

	// TODO: add more classed / interfaces

	if (!strcmp(clsidString, "2FE8F810-B2A5-11D0-A787-0000F803ABFC")) {
		// DEFINE_GUID(CLSID_DirectPlayLobby, 0x2fe8f810, 0xb2a5, 0x11d0, 0xa7, 0x87, 0x0, 0x0, 0xf8, 0x3, 0xab, 0xfc);
		if (!strcmp(iidString, "2DB72491-652C-11D1-A7A8-0000F803ABFC")) {
			name = "IDirectPlayLobby3"; // actually IDirectPlayLobby3A
		}
	} else if (!strcmp(clsidString, "D7B70EE0-4340-11CF-B063-0020AFC2CD35")) {
		// DEFINE_GUID(CLSID_DirectDraw, 0xD7B70EE0,0x4340,0x11CF,0xB0,0x63,0x00,0x20,0xAF,0xC2,0xCD,0x35);
		if (!strcmp(iidString, "9C59509A-39BD-11D1-8C4A-00C04FD930C5")) {
			name = "IDirectDraw4";
		}
	} else if (!strcmp(clsidString, "92FA2C24-253C-11D2-90FB-006008A1F441")) {
		if (!strcmp(iidString, "E4C40280-CCBA-11D2-9DCF-00500411582F")) {
			name = "IA3d4";
		}
	} else if (!strcmp(clsidString, "D1EB6D20-8923-11D0-9D97-00A0C90A43CB")) {
		if (!strcmp(iidString, "0AB1C530-4745-11D1-A7A1-0000F803ABFC")) {
			name = "IDirectPlay4";
		}
	}

	if (name.empty()) {
		fmt::print(stderr, "{}: {} Unknown CLSID={}, IID={}\n",
			fmt::styled("ERROR", fmt::fg(fmt::color::crimson)),
			fmt::styled("CoCreateInstance", fmt::fg(fmt::color::gold)),
			fmt::styled(clsidString, fmt::fg(fmt::color::lime)),
			fmt::styled(iidString, fmt::fg(fmt::color::lime))
		);
		return API__E_FAIL;
	}

	*ppv = game_->CreateInterface(name.c_str(), 200);

	return API__S_OK;
}

////////////////////////////////////////////////////////////////////////////////

Ole32Dll::Ole32Dll(GameEmuInterface* game): DllBase(game) {}
Ole32Dll::~Ole32Dll() {}
void Ole32Dll::Register() {
	game_->ExportAddT<API__CoInitialize>("CoInitialize");
	game_->ExportAddT<API__CoUninitialize>("CoUninitialize");
	game_->ExportAddT<API__CoCreateInstance>("CoCreateInstance");
}

////////////////////////////////////////////////////////////////////////////////
}
