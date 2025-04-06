#pragma once

#include <emulasi/core/export.hpp>
#include <emulasi/core/dllbase.hpp>
#include <emulasi/core/gameemu_interface.hpp>

namespace emulasi
{
////////////////////////////////////////////////////////////////////////////////

class VersionDll: public DllBase {
public:
	VersionDll(GameEmuInterface* game);
	~VersionDll();
	void Register() override;
};

////////////////////////////////////////////////////////////////////////////////
};
