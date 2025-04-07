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
	Ptr32 lpBuffer,  // [out] LPSTR
	Ptr32 pcbBuffer) // [in, out] LPDWORD
{
	const char* name = "DummyUser";
	size_t len = strlen(name) + 1;

#if DBG_PRINT
	fmt::print(stderr, "{:s}(lpBuffer={},pcbBuffer={})\n",
		fmt::styled("GetUserNameA", fmt::fg(fmt::color::gold)),
		fmt::styled(lpBuffer, fmt::fg(fmt::color::aqua)),
		fmt::styled(pcbBuffer, fmt::fg(fmt::color::aqua))
	);
#endif

	if (!pcbBuffer) return (bool)false;

#if DBG_PRINT
	fmt::print(stderr, "-- cbBuffer={}\n", fmt::styled(*pcbBuffer.Mem<uint32_t>(game_), fmt::fg(fmt::color::fuchsia)));
#endif

	if (*pcbBuffer.Mem<uint32_t>(game_) >= len)
	{
		strncpy(lpBuffer.Mem<char>(game_), name, len);
		*pcbBuffer.Mem<uint32_t>(game_) = len;
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
	Ptr32    lpSubKey,             // [in] LPCSTR
	uint32_t _reserved,
	Ptr32    lpClass,              // [in, optional] LPSTR
	uint32_t dwOptions,            // [in] DWORD
	uint32_t samDesired,           // [in] REGSAM
	Ptr32    lpSecurityAttributes, // [in, optional] const LPSECURITY_ATTRIBUTES
	Ptr32    phkResult,            // [out] PHKEY
	Ptr32    lpdwDisposition)      // [out, optional] LPDWORD
{
#if DBG_PRINT
	fmt::print(stderr, "{}(hKey={:#X},",
		fmt::styled("RegCreateKeyExA", fmt::fg(fmt::color::gold)),
		fmt::styled(hKey, fmt::fg(fmt::color::fuchsia))
		);
	fmt::print(stderr, "lpSubKey={},", fmt::styled(lpSubKey, fmt::fg(fmt::color::aqua)));
	fmt::print(stderr, "lpClass={},", fmt::styled(lpClass, fmt::fg(fmt::color::aqua)));
	fmt::print(stderr, "dwOptions={:#x},", fmt::styled(dwOptions, fmt::fg(fmt::color::fuchsia)));
	fmt::print(stderr, "samDesired={:#x},", fmt::styled(samDesired, fmt::fg(fmt::color::fuchsia)));
	fmt::print(stderr, "lpSecurityAttributes={},", fmt::styled(lpSecurityAttributes, fmt::fg(fmt::color::aqua)));
	fmt::print(stderr, "phkResult={},", fmt::styled(phkResult, fmt::fg(fmt::color::aqua)));
	fmt::print(stderr, "lpdwDisposition={})\n", fmt::styled(lpdwDisposition, fmt::fg(fmt::color::aqua)));
#endif

	if (!lpSubKey)
	{
		return API__ERROR_INVALID_PARAMETER;
	}
	if (!phkResult) {
		return API__ERROR_BADKEY;
	}

#if DBG_PRINT
	if (lpSubKey) fmt::print(stderr, "-- szSubKey={}\n", fmt::styled(lpSubKey.Mem<char>(game_), fmt::fg(fmt::color::lime)));
	if (lpClass) fmt::print(stderr, "-- szClass={}\n", fmt::styled(lpClass.Mem<char>(game_), fmt::fg(fmt::color::lime)));
#endif

	auto impl_ = game_->GetRegistryManager();
	std::string key_alias;

	if (impl_->IsSpecialHKEY(hKey, &key_alias)) {
		fmt::print(stderr, "-- hKey={}\n", key_alias);
	} else {
		return API__ERROR_INVALID_HANDLE;
	}

	std::string keyPath = impl_->FullPath(hKey);
	auto newKeyPath = fmt::format("{}\\{}", keyPath, lpSubKey.Mem<char>(game_));
	auto keyResult = impl_->Traverse(newKeyPath, true);

	if (!keyResult) {
		fmt::print(stderr, fmt::fg(fmt::color::crimson), "ERROR: unable to create {:s}\n", newKeyPath);
		return API__ERROR_INVALID_PARAMETER;
	}

	fmt::print(stderr, "-- hkResult={:#x} ('{:s}')\n", keyResult, newKeyPath);

	if (phkResult)
	{
		*phkResult.Mem<uint32_t>(game_) = keyResult;
	}

	return API__ERROR_SUCCESS;
}

/**
 * RegQueryValueExA
 * @return LSTATUS
 */
ExportReturnParam API__RegQueryValueExA(GameEmuInterface * game_,
	uint32_t hKey,        // [in] HKEY
	Ptr32    lpValueName, // [in, optional] LPCSTR
	uint32_t _reserved,
	Ptr32    lpType,      // [out, optional] LPDWORD
	Ptr32    lpData,      // [out, optional] LPBYTE
	Ptr32    lpcbData)    // [in, out, optional] LPDWORD
{
	int returnValue = 0;

#if DBG_PRINT
	fmt::print(stderr, "{}(hKey={:#x},",
		fmt::styled("RegQueryValueExA", fmt::fg(fmt::color::gold)),
		fmt::styled(hKey, fmt::fg(fmt::color::gold)));
	fmt::print(stderr, "lpValueName=0{},",
		fmt::styled(lpValueName, fmt::fg(fmt::color::aqua)));
	fmt::print(stderr, "lpType={},",
		fmt::styled(lpType, fmt::fg(fmt::color::aqua)));
	fmt::print(stderr, "lpData={},",
		fmt::styled(lpData, fmt::fg(fmt::color::aqua)));
	fmt::print(stderr, "lpcbData={})\n",
		fmt::styled(lpcbData, fmt::fg(fmt::color::aqua)));
#endif

#if DBG_PRINT
	if (lpValueName) fmt::print(stderr, "-- szValueName={}\n", fmt::styled(lpValueName.Mem<char>(game_), fmt::fg(fmt::color::lime)));
#endif
	return API__ERROR_FILE_NOT_FOUND;
}

/**
 * RegSetValueExA
 * @return LSTATUS
 */
ExportReturnParam API__RegSetValueExA(GameEmuInterface * game_,
	uint32_t hKey,        // [in] HKEY
	Ptr32    lpValueName, // [in, optional] LPCSTR
	uint32_t _reserved,
	uint32_t dwType,      // [in] DWORD
	Ptr32    lpData,      // [in] const PBYTE
	uint32_t cbData)      // [in] DWORD
{
#if DBG_PRINT
	fmt::print(stderr, "{}(hKey={:#x},",
		fmt::styled("RegSetValueExA", fmt::fg(fmt::color::gold)),
		fmt::styled(hKey, fmt::fg(fmt::color::gold)));
	fmt::print(stderr, "lpValueName={},",
		fmt::styled(lpValueName, fmt::fg(fmt::color::aqua)));
	fmt::print(stderr, "dwType={:#x},",
		fmt::styled(dwType, fmt::fg(fmt::color::fuchsia)));
	fmt::print(stderr, "lpData={},",
		fmt::styled(lpData, fmt::fg(fmt::color::aqua)));
	fmt::print(stderr, "cbData={:d})\n",
		fmt::styled(cbData, fmt::fg(fmt::color::fuchsia)));
#endif

#if DBG_PRINT
	if (lpValueName) fmt::print(stderr, "-- szValueName={}\n",
		fmt::styled(lpValueName.Mem<char>(game_), fmt::fg(fmt::color::lime)));
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
	Ptr32 lpSubKey,      // [in, optional] LPCSTR
	uint32_t ulOptions,  // [in] DWORD
	uint32_t samDesired, // [in] REGSAM
	Ptr32 phkResult)     // [out] PHKEY
{
#if DBG_PRINT
	fmt::print(stderr, "{}(hKey={:#x},", fmt::styled("RegOpenKeyExA",
		fmt::fg(fmt::color::gold)),
		fmt::styled(hKey, fmt::fg(fmt::color::fuchsia)));
	fmt::print(stderr, "lpSubKey={},",
		fmt::styled(lpSubKey, fmt::fg(fmt::color::aqua)));
	fmt::print(stderr, "ulOptions={:#x},",
		fmt::styled(ulOptions, fmt::fg(fmt::color::fuchsia)));
	fmt::print(stderr, "samDesired={:#x},",
		fmt::styled(samDesired, fmt::fg(fmt::color::fuchsia)));
	fmt::print(stderr, "phkResult={})\n",
		fmt::styled(phkResult, fmt::fg(fmt::color::aqua)));
#endif

	if (!lpSubKey || !*lpSubKey.Mem<char>(game_)) {
		if (phkResult) {
			*phkResult.Mem<uint32_t>(game_) = hKey;
			return API__ERROR_SUCCESS;
		}
	}

#if DBG_PRINT
	if (lpSubKey) fmt::print(stderr, "-- szSubKey={}\n", fmt::styled(lpSubKey.Mem<char>(game_), fmt::fg(fmt::color::lime)));
#endif
	auto impl_ = game_->GetRegistryManager();
	std::string keyAlias;
	if (impl_->IsSpecialHKEY(hKey, &keyAlias)) {
		fmt::print(stderr, "-- hKey={}\n", keyAlias);
	} else {
		return API__ERROR_INVALID_HANDLE;
	}

	std::string keyPath = impl_->FullPath(hKey);
	auto openedKeyPath = fmt::format("{}\\{}", keyPath, lpSubKey.Mem<char>(game_));
	auto keyResult = impl_->Traverse(openedKeyPath, false);

	if (!keyResult) {
		return API__ERROR_FILE_NOT_FOUND;
	}

	fmt::print(stderr, "-- hkResult={:#x} ('{:s}')\n", keyResult, openedKeyPath);

	if (phkResult) {
		*phkResult.Mem<uint32_t>(game_) = keyResult;
	}
	return API__ERROR_SUCCESS;
}

/**
 * RegOpenKeyA
 * @return LSTATUS
 */
ExportReturnParam API__RegOpenKeyA(GameEmuInterface * game_,
	uint32_t hKey,   // [in] HKEY
	Ptr32 lpSubKey,  // [in, optional] LPCSTR
	Ptr32 phkResult) // [out] PHKEY
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
