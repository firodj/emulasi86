#include "emulasi/core/ms_windows.h"
#include "emulasi/core/export.hpp"
#include "emulasi/core/emulation.hpp"
#include "gdi32.hpp"

#include <string.h>
#include <cassert>
#include <functional>
#include <fmt/core.h>
#include <fmt/color.h>
#include "emulasi/ui/gameemu.hpp"
#include <emulasi/core/gameemu_interface.hpp>
#include "emulasi/util/log.hpp"

namespace emulasi
{
////////////////////////////////////////////////////////////////////////////////

/**
 * GetStockObject
 * @return HGDIOBJ
 */
ExportReturnParam API__GetStockObject(GameEmuInterface *game_,
	uint32_t i)  // [in] int
{
	int returnValue = 0;

	fmt::print(stderr, "{}(i={})\n",
		fmt::styled("GetStockObject", fmt::fg(fmt::color::gold)), i);

	return returnValue;
}

/**
 * GetObjectA
 * @return int
 */
ExportReturnParam API__GetObjectA(GameEmuInterface *game_,
	uint32_t h, // [in] HANDLE
	int        c, // [in]
	uint32_t * pv // [out] LPVOID
)
{
	int returnValue = 0;

	fmt::print(stderr, "{}(h={:#x},c={},pv={})\n",
		fmt::styled("GetObjectA", fmt::fg(fmt::color::gold)),
		h, c, fmt::ptr(pv));

	memcpy(pv, game_->Memory(h), c);

	return returnValue;
}

/**
 * CreateCompatibleDC

);
 * @return HDC
 * @remark Gdi32.lib
 */
ExportReturnParam API__CreateCompatibleDC(GameEmuInterface *game_,
	uint32_t hdc) // [in] HDC
{
	int returnValue = 0;

	fmt::print(stderr, "{}(hdc={:#x})",
		fmt::styled("CreateCompatibleDC", fmt::fg(fmt::color::gold)), hdc);

	returnValue = game_->Allocate(sizeof(uint32_t));

	return returnValue;
}

/**
 * HGDIOBJ SelectObject
 * @return HGDIOBJ
 * @remark Gdi32.lib
 */
ExportReturnParam API__SelectObject(GameEmuInterface *game_,
	uint32_t hdc, // [in] HDC
	uint32_t h) // [in] HGDIOBJ
{
	int returnValue = 0;
	fmt::print(stderr, "{}(hdc={},h={})\n",
		fmt::styled("SelectObject", fmt::fg(fmt::color::gold)), hdc, h);
	*(uint32_t*)game_->Memory(hdc) = h;
	return returnValue;
}

/**
 * StretchBlt
 * @return BOOL
 * @remark Gdi32.lib
 */
ExportReturnParam API__StretchBlt(GameEmuInterface *game_,
	uint32_t hdcDest, // [in] HDC
	int32_t xDest, // [in] int
	int32_t yDest, // [in] int
	int32_t wDest, // [in] int
	int32_t hDest, // [in] int
	uint32_t hdcSrc, // [in] HDC
	int32_t xSrc, // [in] int
	int32_t ySrc, // [in] int
	int32_t wSrc, // [in] int
	int32_t hSrc, // [in] int
	uint32_t rop) // [in] DWORD
{
	int returnValue = 0;

	fmt::print(stderr, "{}(hdcDest={:#x},xDest={},yDest={},wDest={},hDest={},",
		fmt::styled("StretchBlt", fmt::fg(fmt::color::gold)),
		hdcDest, xDest, yDest, wDest, hDest
	);
	fmt::print(stderr, "hdcSrc={:#x},xSrc={},ySrc={},wSrc={},hSrc={},rop={})\n",
		hdcSrc, xSrc, ySrc, wSrc, hSrc, rop
	);

	uint32_t objectAddress = *(uint32_t*)game_->Memory(hdcSrc);
	API__BITMAP* bitmap = (API__BITMAP*) game_->Memory(objectAddress);
	void* data = game_->Memory(bitmap->bmBits);

	game_->GetGfx()->StretchBlt(hdcDest, wDest, hDest, data);

	returnValue = 1;
	return returnValue;
}

/**
 * DeleteDC
 * @return BOOL
 * @remark Gdi32.lib
 */
ExportReturnParam API__DeleteDC(GameEmuInterface *game_,
	uint32_t hdc) // [in] HDC
{
	int returnValue = 0;

	fmt::print(stderr, "{}(hdc={:#x})\n",
		fmt::styled("DeleteDC", fmt::fg(fmt::color::gold)), hdc);

	returnValue = 1;

	return returnValue;
}

/**
 * DeleteObject
 * @return BOOL
 * @remark Gdi32.lib
 */
ExportReturnParam API__DeleteObject(GameEmuInterface *game_,
	uint32_t ho) // [in] HGDIOBJ
{
	int returnValue = 0;

	fmt::print(stderr, "{}(hdc={:#x})\n",
		fmt::styled("DeleteObject", fmt::fg(fmt::color::gold)), ho);

	returnValue = 1;

	return returnValue;
}

/**
 * GetPixel
 * @return COLORREF
 * @remark Gdi32.lib
 */
ExportReturnParam API__GetPixel(GameEmuInterface *game_,
	uint32_t hdc, // [in] HDC
	int32_t x, // [in] int
	int32_t y // [in] int
)
{
	int returnValue = 0;

	fmt::print(stderr, "{}(hdc={:#x},x={},y={})\n",
		fmt::styled("GetPixel", fmt::fg(fmt::color::gold)),
		hdc, x, y);

	returnValue = 0xFF00FF00 ^ x ^ y;

	return returnValue;
}

/**
 * CreateFontA
 * @return HFONT
 * @remark Gdi32.lib
 */
ExportReturnParam API__CreateFontA(GameEmuInterface *game_,
	int32_t cHeight, // [in] int
	int32_t cWidth, // [in] int
	int32_t cEscapement, // [in] int
	int32_t cOrientation, // [in] int
	int32_t cWeight, // [in] int
	uint32_t bItalic, // [in] DWORD
	uint32_t bUnderline, // [in] DWORD
	uint32_t bStrikeOut, // [in] DWORD
	uint32_t iCharSet, // [in] DWORD
	uint32_t iOutPrecision, // [in] DWORD
	uint32_t iClipPrecision, // [in] DWORD
	uint32_t iQuality, // [in] DWORD
	uint32_t iPitchAndFamily, // [in] DWORD
	char* pszFaceName) // [in] LPCSTR
{
	int returnValue = 0;

	fmt::print(stderr, "{}(cHeight={},cWidth={},cEscapement={},cOrientation={},cWeight={},",
		fmt::styled("CreateFontA", fmt::fg(fmt::color::gold)),
		cHeight, cWidth, cEscapement, cOrientation, cWeight
	);
	fmt::print(stderr, "bItalic={},bUnderline={},bStrikeOut={},iCharSet={},",
		bItalic, bUnderline, bStrikeOut, iCharSet
	);
	fmt::print(stderr, "iOutPrecision={},iClipPrecision={},iQuality={},iPitchAndFamily={},",
		iOutPrecision, iClipPrecision, iQuality, iPitchAndFamily);
	fmt::print(stderr,"pszFaceName={})\n", fmt::ptr(pszFaceName));

	if (pszFaceName) {
		fmt::print(stderr, "-- szFaceName={:s}\n",
			fmt::styled(pszFaceName, fmt::fg(fmt::color::lime)));
	}

	returnValue = 0x1337C0DE;

	return returnValue;
}

////////////////////////////////////////////////////////////////////////////////
Gdi32Dll::Gdi32Dll(GameEmuInterface* game): DllBase(game) {}
Gdi32Dll::~Gdi32Dll() {}
void Gdi32Dll::Register()
{

	game_->ExportAddT<API__GetStockObject>("GetStockObject");
	game_->ExportAddT<API__GetObjectA>("GetObjectA");
	game_->ExportAddT<API__CreateCompatibleDC>("CreateCompatibleDC");
	game_->ExportAddT<API__SelectObject>("SelectObject");
	game_->ExportAddT<API__StretchBlt>("StretchBlt");
	game_->ExportAddT<API__DeleteDC>("DeleteDC");
	game_->ExportAddT<API__DeleteObject>("DeleteObject");
	game_->ExportAddT<API__GetPixel>("GetPixel");
	game_->ExportAddT<API__CreateFontA>("CreateFontA");
}

////////////////////////////////////////////////////////////////////////////////
