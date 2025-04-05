#pragma once

#include <emulasi/core/export.hpp>
#include <emulasi/core/dllbase.hpp>
#include <emulasi/core/gameemu_interface.hpp>

namespace emulasi
{
////////////////////////////////////////////////////////////////////////////////
class ComCtl32Dll: public DllBase {
public:
	ComCtl32Dll(GameEmuInterface* game);
	~ComCtl32Dll();
	void Register() override;
};
////////////////////////////////////////////////////////////////////////////////
};
