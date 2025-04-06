
#pragma once

#include <emulasi/core/export.hpp>
#include <emulasi/core/dllbase.hpp>
#include <emulasi/core/gameemu_interface.hpp>

namespace emulasi {
////////////////////////////////////////////////////////////////////////////////
class Ole32Dll: public DllBase {
public:
	Ole32Dll(GameEmuInterface* game);
	~Ole32Dll();
	void Register() override;
};
////////////////////////////////////////////////////////////////////////////////
};
