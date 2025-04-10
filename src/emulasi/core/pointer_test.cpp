#include <gtest/gtest.h>
#include "pointer.hpp"
#include "export.hpp"
#include "dllbase.hpp"
#include <fmt/color.h>
#include "gameemuabstract.hpp"

using namespace emulasi;

TEST(Ptr32Test, SizeOf)
{
	ASSERT_EQ(4, sizeof(Ptr32));
}

TEST(Ptr32Test, AsBool)
{
	Ptr32 nonNullPtr(1);

	ASSERT_TRUE(nonNullPtr);

	Ptr32 nullPtr(0);

	ASSERT_FALSE(nullPtr);
}

TEST(Ptr32Test, AsUint32)
{
	Ptr32 someValue(128);

	ASSERT_EQ((uint32_t)someValue, 128);
}

TEST(Ptr32Test, ExportReturnParam)
{
	Ptr32 someValue(512);

	ExportReturnParam returnValue{someValue};

	ASSERT_EQ(returnValue.retVal(), 512);
}

#if 1
TEST(Ptr32Test, Mem)
{
	class FakeMem: public GameEmuAbstract {
	public:
		FakeMem(): data(64) {}
 		void* Memory(uint32_t address) override {
			if (address == 0x1000)
				return &data;
			return nullptr;
		}
		uint32_t data;
	};

	FakeMem gameEmu;

	Ptr32 someValue(0x1000);

	uint32_t *realPtr = someValue.Mem<uint32_t>(&gameEmu);

	ASSERT_NE(realPtr, nullptr);
	ASSERT_EQ(*realPtr, 64);

}
#endif

#if 1
TEST(Ptr32Test, FormatAs)
{
	Ptr32 ptr(384);

	EXPECT_EQ(fmt::format("{}", ptr), "0x00000180");

	fmt::print(stderr, "value = {}\n", fmt::styled(ptr, fmt::fg(fmt::color::aqua)));
}
