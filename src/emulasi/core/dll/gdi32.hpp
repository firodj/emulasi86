#pragma once

#include "emulasi/core/export.hpp"
#include <emulasi/core/dllbase.hpp>
#include <emulasi/core/gameemu_interface.hpp>

namespace emulasi
{
////////////////////////////////////////////////////////////////////////////////

class Gdi32Dll: public DllBase {
public:
	Gdi32Dll(GameEmuInterface* game);
	~Gdi32Dll();
	void Register() override;
};
////////////////////////////////////////////////////////////////////////////////
