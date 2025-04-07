#include "emulasi/core/ms_windows.h"
#include "emulasi/core/export.hpp"
#include "emulasi/core/emulation.hpp"
#include "gdi32.hpp"

#include <glad/gl.h>
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
	Address h, // [in] HANDLE
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
	Address hdc) // [in] HDC
{
	int returnValue = 0;

	fmt::print(stderr, "{}(hdc={:#x})",
		fmt::styled("CreateCompatibleDC", fmt::fg(fmt::color::gold)), hdc);

	returnValue = game_->Allocate(sizeof(Address));

	return returnValue;
}

/**
 * HGDIOBJ SelectObject
 * @return HGDIOBJ
 * @remark Gdi32.lib
 */
ExportReturnParam API__SelectObject(GameEmuInterface *game_,
	Address hdc, // [in] HDC
	Address h) // [in] HGDIOBJ
{
	int returnValue = 0;
	fmt::print(stderr, "{}(hdc={},h={})\n",
		fmt::styled("SelectObject", fmt::fg(fmt::color::gold)), hdc, h);
	*(Address*)game_->Memory(hdc) = h;
	return returnValue;
}

/**
 * StretchBlt
 * @return BOOL
 * @remark Gdi32.lib
 */
ExportReturnParam API__StretchBlt(GameEmuInterface *game_,
	Address hdcDest, // [in] HDC
	int32_t xDest, // [in] int
	int32_t yDest, // [in] int
	int32_t wDest, // [in] int
	int32_t hDest, // [in] int
	Address hdcSrc, // [in] HDC
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

	// Get the pointer to the object the DC points at, we'll assume that it is a BITMAP
	Address objectAddress = *(Address*)game_->Memory(hdcSrc);
	API__BITMAP* bitmap = (API__BITMAP*) game_->Memory(objectAddress);
	void* data = game_->Memory(bitmap->bmBits);

	// Update the texture interface
	if (hdcDest != 0) {
		GLint previousTexture = 0;
		glGetIntegerv(GL_TEXTURE_BINDING_2D, &previousTexture);
		glBindTexture(GL_TEXTURE_2D, hdcDest);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, wDest, hDest, 0, GL_BGR, GL_UNSIGNED_BYTE, data);
		glBindTexture(GL_TEXTURE_2D, previousTexture);
	} else {
		glClearColor(1.0f,0.0f,1.0f,1.0f);
		glClear(GL_COLOR_BUFFER_BIT);
		fmt::print(stderr, "ERROR: invalid destination!\n");
	}

	returnValue = 1;
	return returnValue;
}

/**
 * DeleteDC
 * @return BOOL
 * @remark Gdi32.lib
 */
ExportReturnParam API__DeleteDC(GameEmuInterface *game_,
	Address hdc) // [in] HDC
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
	Address ho) // [in] HGDIOBJ
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
	Address hdc, // [in] HDC
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
