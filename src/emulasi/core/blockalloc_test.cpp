#include <gtest/gtest.h>
#include <fmt/core.h>

#include "blockalloc.hpp"

using namespace emulasi;

class BlockAllocatorTest: public testing::Test
{
protected:
	BlockAllocatorTest() {}
	~BlockAllocatorTest() override {}
	void SetUp() override {}
	void TearDown() override {}
};

TEST_F(BlockAllocatorTest, allocatedSize)
{
	BlockAllocator blockAllocator(0x40000000, 0x1000);

	int ptr_data = blockAllocator.Allocate(0x512);
	int sz = blockAllocator.AllocatedSize(ptr_data);

	ASSERT_EQ(0x1000, ptr_data);
	ASSERT_EQ(0x1000, sz);

	int ptr_data2 = blockAllocator.Allocate(0x1512);
	sz = blockAllocator.AllocatedSize(ptr_data2);

	ASSERT_EQ(0x3000, ptr_data2);
	ASSERT_EQ(0x2000, sz);
}
