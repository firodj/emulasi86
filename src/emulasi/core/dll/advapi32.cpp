#include "emulasi/core/ms_windows.h"
#include "advapi32.hpp"
#include <emulasi/core/registry.hpp>
#include <string.h>
#include <cassert>
#include <fmt/core.h>
#include <fmt/color.h>

#define DBG_PRINT 1

namespace emulasi {
////////////////////////////////////////////////////////////////////////////////

/**
 * RegCloseKey
 * @returnn LSTATUS
 */
ExportReturnParam API__RegCloseKey(GameEmuInterface * game,
	uint32_t hKey) // [in] HKEY
{
	int returnValue = 0;

#if DBG_PRINT
	fmt::print(stderr, "{}(hKey={:#x})\n",
		fmt::styled("RegCloseKey", fmt::fg(fmt::color::gold)),
		fmt::styled(hKey, fmt::fg(fmt::color::fuchsia))
		);
#endif

	return API__ERROR_SUCCESS;
}

/**
 * GetUserNameA
 * @return BOOL
 */
ExportReturnParam API__GetUserNameA(GameEmuInterface * game_,
	char* lpBuffer,      // [out] LPSTR
	uint32_t* pcbBuffer) // [in, out] LPDWORD
{
	const char* name = "DummyUser";
	size_t len = strlen(name) + 1;

#if DBG_PRINT
	fmt::print(stderr, "{:s}(lpBuffer={},pcbBuffer={})\n",
		fmt::styled("GetUserNameA", fmt::fg(fmt::color::gold)),
		fmt::styled(fmt::ptr(lpBuffer), fmt::fg(fmt::color::aqua)),
		fmt::styled(fmt::ptr(pcbBuffer), fmt::fg(fmt::color::aqua))
	);
#endif

	if (!pcbBuffer) return (bool)false;

#if DBG_PRINT
	fmt::print(stderr, "-- cbBuffer={}\n", fmt::styled(*pcbBuffer, fmt::fg(fmt::color::fuchsia)));
#endif

	if (*pcbBuffer >= len)
	{
		strncpy(lpBuffer, name, len);
		*pcbBuffer = len;
	}
	else
	{
		// API__SetLastError(ERROR_INSUFFICIENT_BUFFER);
		return (bool)false;
	}

	return (bool)true;
}

/**
 * RegCreateKeyExA
 * @return LSTATUS
 */
ExportReturnParam API__RegCreateKeyExA(GameEmuInterface * game_,
	uint32_t hKey,                 // [in] HKEY
	char* lpSubKey,                // [in] LPCSTR
	uint32_t _reserved,
	char* lpClass,                 // [in, optional] LPSTR
	uint32_t dwOptions,            // [in] DWORD
	uint32_t samDesired,           // [in] REGSAM
	void* lpSecurityAttributes,    // [in, optional] const LPSECURITY_ATTRIBUTES
	uint32_t *phkResult,           // [out] PHKEY
	uint32_t *lpdwDisposition)     // [out, optional] LPDWORD
{
#if DBG_PRINT
	fmt::print(stderr, "{}(hKey={:#X},",
		fmt::styled("RegCreateKeyExA", fmt::fg(fmt::color::gold)),
		fmt::styled(hKey, fmt::fg(fmt::color::fuchsia))
		);
	fmt::print(stderr, "lpSubKey={},", fmt::styled(fmt::ptr(lpSubKey), fmt::fg(fmt::color::aqua)));
	fmt::print(stderr, "lpClass={},", fmt::styled(fmt::ptr(lpClass), fmt::fg(fmt::color::aqua)));
	fmt::print(stderr, "dwOptions={:#x},", fmt::styled(dwOptions, fmt::fg(fmt::color::fuchsia)));
	fmt::print(stderr, "samDesired={:#x},", fmt::styled(samDesired, fmt::fg(fmt::color::fuchsia)));
	fmt::print(stderr, "lpSecurityAttributes={},", fmt::styled(fmt::ptr(lpSecurityAttributes), fmt::fg(fmt::color::aqua)));
	fmt::print(stderr, "phkResult={},", fmt::styled(fmt::ptr(phkResult), fmt::fg(fmt::color::aqua)));
	fmt::print(stderr, "lpdwDisposition={})\n", fmt::styled(fmt::ptr(lpdwDisposition), fmt::fg(fmt::color::aqua)));
#endif

	if (!lpSubKey)
	{
		return API__ERROR_INVALID_PARAMETER;
	}
	if (!phkResult) {
		return API__ERROR_BADKEY;
	}

#if DBG_PRINT
	if (lpSubKey) fmt::print(stderr, "-- szSubKey={}\n", fmt::styled((char*)lpSubKey, fmt::fg(fmt::color::lime)));
	if (lpClass) fmt::print(stderr, "-- szClass={}\n", fmt::styled((char*)lpClass, fmt::fg(fmt::color::lime)));
#endif

	auto impl_ = game_->GetRegistryManager();
	std::string key_alias;

	if (impl_->IsSpecialHKEY(hKey, &key_alias)) {
		fmt::print(stderr, "-- hKey={}\n", key_alias);
	} else {
		return API__ERROR_INVALID_HANDLE;
	}

	std::string key_path = impl_->FullPath(hKey);
	auto new_key_path = fmt::format("{}\\{}", key_path, (char*)lpSubKey);
	auto keyResult = impl_->Traverse(new_key_path, true);

	if (!keyResult) {
		fmt::print(stderr, fmt::fg(fmt::color::crimson), "ERROR: unable to create {:s}\n", new_key_path);
		return API__ERROR_INVALID_PARAMETER;
	}

	fmt::print(stderr, "-- hkResult={:#x} ('{:s}')\n", keyResult, new_key_path);

	if (phkResult != 0)
	{
		*phkResult = keyResult;
	}

	return API__ERROR_SUCCESS;
}

/**
 * RegQueryValueExA
 * @return LSTATUS
 */
ExportReturnParam API__RegQueryValueExA(GameEmuInterface * game_,
	uint32_t hKey, // [in] HKEY
	char *lpValueName, // [in, optional] LPCSTR
	uint32_t _reserved,
	uint32_t *lpType, // [out, optional] LPDWORD
	uint8_t *lpData, // [out, optional] LPBYTE
	uint32_t *lpcbData) // [in, out, optional] LPDWORD
{
	int returnValue = 0;

#if DBG_PRINT
	fmt::print(stderr, "{}(hKey={:#x},",
		fmt::styled("RegQueryValueExA", fmt::fg(fmt::color::gold)),
		fmt::styled(hKey, fmt::fg(fmt::color::gold)));
	fmt::print(stderr, "lpValueName=0{},",
		fmt::styled(fmt::ptr(lpValueName), fmt::fg(fmt::color::aqua)));
	fmt::print(stderr, "lpType={},",
		fmt::styled(fmt::ptr(lpType), fmt::fg(fmt::color::aqua)));
	fmt::print(stderr, "lpData={},",
		fmt::styled(fmt::ptr(lpData), fmt::fg(fmt::color::aqua)));
	fmt::print(stderr, "lpcbData={})\n",
		fmt::styled(fmt::ptr(lpcbData), fmt::fg(fmt::color::aqua)));
#endif

#if DBG_PRINT
	if (lpValueName) fmt::print(stderr, "-- szValueName={}\n", fmt::styled((char*)lpValueName, fmt::fg(fmt::color::lime)));
#endif
	return API__ERROR_FILE_NOT_FOUND;
}

/**
 * RegSetValueExA
 * @return LSTATUS
 */
ExportReturnParam API__RegSetValueExA(GameEmuInterface * game_,
	uint32_t hKey,     // [in] HKEY
	char *lpValueName, // [in, optional] LPCSTR
	uint32_t _reserved,
	uint32_t dwType,   // [in] DWORD
	uint8_t *lpData,   // [in] const PBYTE
	uint32_t cbData)   // [in] DWORD
{
#if DBG_PRINT
	fmt::print(stderr, "{}(hKey={:#x},",
		fmt::styled("RegSetValueExA", fmt::fg(fmt::color::gold)),
		fmt::styled(hKey, fmt::fg(fmt::color::gold)));
	fmt::print(stderr, "lpValueName={},",
		fmt::styled(fmt::ptr(lpValueName), fmt::fg(fmt::color::aqua)));
	fmt::print(stderr, "dwType={:#x},",
		fmt::styled(dwType, fmt::fg(fmt::color::fuchsia)));
	fmt::print(stderr, "lpData={},",
		fmt::styled(fmt::ptr(lpData), fmt::fg(fmt::color::aqua)));
	fmt::print(stderr, "cbData={:d})\n",
		fmt::styled(cbData, fmt::fg(fmt::color::fuchsia)));
#endif

#if DBG_PRINT
	if (lpValueName) fmt::print(stderr, "-- szValueName={}\n", fmt::styled((char*)lpValueName, fmt::fg(fmt::color::lime)));
#endif

	return API__ERROR_SUCCESS;
}

// Copy protection
/**
 * RegOpenKeyExA
 * @return LSTATUS
 */
ExportReturnParam API__RegOpenKeyExA(GameEmuInterface * game_,
	uint32_t hKey,       // [in] HKEY
	char *lpSubKey,      // [in, optional] LPCSTR
	uint32_t ulOptions,  // [in] DWORD
	uint32_t samDesired, // [in] REGSAM
	uint32_t *phkResult) // [out] PHKEY
{
#if DBG_PRINT
	fmt::print(stderr, "{}(hKey={:#x},", fmt::styled("RegOpenKeyExA",
		fmt::fg(fmt::color::gold)),
		fmt::styled(hKey, fmt::fg(fmt::color::fuchsia)));
	fmt::print(stderr, "lpSubKey={},",
		fmt::styled(fmt::ptr(lpSubKey), fmt::fg(fmt::color::aqua)));
	fmt::print(stderr, "ulOptions={:#x},",
		fmt::styled(ulOptions, fmt::fg(fmt::color::fuchsia)));
	fmt::print(stderr, "samDesired={:#x},",
		fmt::styled(samDesired, fmt::fg(fmt::color::fuchsia)));
	fmt::print(stderr, "phkResult={})\n",
		fmt::styled(fmt::ptr(phkResult), fmt::fg(fmt::color::aqua)));
#endif

	if (!lpSubKey || !*lpSubKey) {
		if (phkResult) {
			*phkResult = hKey;
			return API__ERROR_SUCCESS;
		}
	}

#if DBG_PRINT
	if (lpSubKey) fmt::print(stderr, "-- szSubKey={}\n", fmt::styled((char*)lpSubKey, fmt::fg(fmt::color::lime)));
#endif
	auto impl_ = game_->GetRegistryManager();
	std::string key_alias;
	if (impl_->IsSpecialHKEY(hKey, &key_alias)) {
		fmt::print(stderr, "-- hKey={}\n", key_alias);
	} else {
		return API__ERROR_INVALID_HANDLE;
	}

	std::string key_path = impl_->FullPath(hKey);
	auto open_key_path = fmt::format("{}\\{}", key_path, (char*)lpSubKey);
	auto keyResult = impl_->Traverse(open_key_path, false);

	if (!keyResult) {
		return API__ERROR_FILE_NOT_FOUND;
	}

	fmt::print(stderr, "-- hkResult={:#x} ('{:s}')\n", keyResult, open_key_path);

	if (phkResult) {
		*phkResult = keyResult;
	}
	return API__ERROR_SUCCESS;
}

/**
 * RegOpenKeyA
 * @return LSTATUS
 */
ExportReturnParam API__RegOpenKeyA(GameEmuInterface * game_,
	uint32_t hKey,       // [in] HKEY
	char *lpSubKey,      // [in, optional] LPCSTR
	uint32_t *phkResult) // [out] PHKEY
{
	if (!phkResult) {
		return API__ERROR_INVALID_PARAMETER;
	}

	return API__RegOpenKeyExA(game_, hKey, lpSubKey, 0, 0, phkResult);
#if 0
#if DBG_PRINT
	fmt::print(stderr, "{}(hKey={:#x},", fmt::styled("RegOpenKeyA",
		fmt::fg(fmt::color::gold)),
		fmt::styled(hKey, fmt::fg(fmt::color::fuchsia)));
	fmt::print(stderr, "lpSubKey={},",
		fmt::styled(fmt::ptr(lpSubKey), fmt::fg(fmt::color::aqua)));
	fmt::print(stderr, "phkResult={})\n",
		fmt::styled(fmt::ptr(phkResult), fmt::fg(fmt::color::aqua)));
#endif

#if DBG_PRINT
	if (lpSubKey) fmt::print(stderr, "-- szSubKey={}\n", fmt::styled((char*)lpSubKey, fmt::fg(fmt::color::lime)));
#endif
#endif
	return API__ERROR_SUCCESS;
}

////////////////////////////////////////////////////////////////////////////////

AdvApi32Dll::AdvApi32Dll(GameEmuInterface * game): DllBase(game) {
}

AdvApi32Dll::~AdvApi32Dll() {}

void AdvApi32Dll::Register()
{
	game_->ExportAddT<API__RegCreateKeyExA>("RegCreateKeyExA");
	game_->ExportAddT<API__RegQueryValueExA>("RegQueryValueExA");
	game_->ExportAddT<API__RegSetValueExA>("RegSetValueExA");
	game_->ExportAddT<API__RegOpenKeyExA>("RegOpenKeyExA");
	game_->ExportAddT<API__RegOpenKeyA>("RegOpenKeyA");
	game_->ExportAddT<API__RegCloseKey>("RegCloseKey");
	game_->ExportAddT<API__GetUserNameA>("GetUserNameA");
}

////////////////////////////////////////////////////////////////////////////////
};
