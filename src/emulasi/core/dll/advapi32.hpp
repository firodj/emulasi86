#pragma once

#include <memory>

#include <emulasi/core/export.hpp>
#include <emulasi/core/dllbase.hpp>
#include <emulasi/core/gameemu_interface.hpp>

namespace emulasi
{
////////////////////////////////////////////////////////////////////////////////
class AdvApi32Dll: public DllBase {
public:
	AdvApi32Dll(GameEmuInterface* game);
	~AdvApi32Dll();
	void Register() override;
};

////////////////////////////////////////////////////////////////////////////////
