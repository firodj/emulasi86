#pragma once

#include <emulasi/core/export.hpp>
#include <emulasi/core/dllbase.hpp>
#include <emulasi/core/gameemu_interface.hpp>

namespace emulasi
{
////////////////////////////////////////////////////////////////////////////////

class WinMmDll: public DllBase {
public:
	WinMmDll(GameEmuInterface* game);
	~WinMmDll();
	void Register() override;
};

////////////////////////////////////////////////////////////////////////////////
};
