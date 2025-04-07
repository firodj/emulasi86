#pragma once

#include <cstdint>
#include "gameemu_interface.hpp"
#include <fmt/core.h>

namespace emulasi {
////////////////////////////////////////////////////////////////////////////////

#pragma pack(push, 1)
struct Ptr32 {
	uint32_t val;

	Ptr32(uint32_t init_val): val(init_val) {}
	operator unsigned int() const { return val; }
	operator bool() const { return val != 0; }

	template <typename T>
	T* Mem(GameEmuInterface * gameEmu)
	{
		return (T*)gameEmu->Memory(val);
	}
};
#pragma pack(pop)


////////////////////////////////////////////////////////////////////////////////
};

template <> struct fmt::formatter<emulasi::Ptr32>: formatter<string_view> {
	// parse is inherited from formatter<string_view>.
	auto format(emulasi::Ptr32 ptr, format_context& ctx) const -> decltype(ctx.out());
};
