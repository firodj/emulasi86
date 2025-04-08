#pragma once

#include "emulasi/core/ms_windows.h"
#include "emulasi/core/export.hpp"
#include <emulasi/core/dllbase.hpp>
#include <emulasi/core/gameemu_interface.hpp>

namespace emulasi
{
////////////////////////////////////////////////////////////////////////////////
class User32Dll: public DllBase {
public:
    User32Dll(GameEmuInterface* game);
    ~User32Dll();

    void Register() override;

    ExportReturnParam wsprintfA(ExportStackParam* param);
};

////////////////////////////////////////////////////////////////////////////////
};
