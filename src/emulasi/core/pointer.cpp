#include "pointer.hpp"
#include <fmt/color.h>

auto fmt::formatter<emulasi::Ptr32>::format(emulasi::Ptr32 ptr, format_context& ctx) const -> decltype(ctx.out()) {
	fmt::format_to(ctx.out(), "{:#010x}", ptr.val);
	return ctx.out();
}
