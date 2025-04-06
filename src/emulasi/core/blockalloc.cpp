#include <cstdlib>
#include <cstring>
#include <cstdint>
#include <libassert/assert.hpp>

#include "blockalloc.hpp"
#include "memalign.hpp"

#define DBG_PAD    1
#define DBG_EXPAND 2

namespace emulasi
{
////////////////////////////////////////////////////////////////////////////////

BlockAllocator::BlockAllocator(unsigned int size, unsigned int blockSize)
{
	DEBUG_ASSERT(size % blockSize == 0);
	m_blockSize  = blockSize;
	m_blockCount = size / m_blockSize;
	size_t usageByteCount = (m_blockCount + 7) / 8;
	m_blockUsage = std::unique_ptr<uint8_t[]>(new uint8_t[usageByteCount]);
	std::memset(m_blockUsage.get(), 0x00, usageByteCount);
}

BlockAllocator::~BlockAllocator()
{
}

void BlockAllocator::MarkBlockUsed(unsigned int block, bool used)
{
	uint8_t mask = 1 << (block % 8);
	if (used) {
		m_blockUsage[block / 8] |= mask;
	} else {
		m_blockUsage[block / 8] &= ~mask;
	}
}

bool BlockAllocator::IsBlockUsed(unsigned int block)
{
	uint8_t mask = 1 << (block % 8);
	return m_blockUsage[block / 8] & mask;
}

unsigned int BlockAllocator::CountBlocks(unsigned int block, bool used)
{
	unsigned int count = 0;
	while (block < m_blockCount) {
		if (IsBlockUsed(block) != used) {
			break;
		}
		block++;
		count++;
	}
	return count;
}

unsigned int BlockAllocator::FindFreeBlocks(unsigned int needed_blocks)
{
	unsigned int bestCount = 0;
	unsigned int bestBlock = 0;
	unsigned int block = 0;
	while(block < m_blockCount) {
		bool blockUsed = IsBlockUsed(block);
		unsigned int availableBlocks = CountBlocks(block, blockUsed);

		if (!blockUsed && (availableBlocks >= needed_blocks)) {
			if ((bestCount == 0) || (availableBlocks < bestCount)) {
				bestBlock = block;
				bestCount = availableBlocks;
				if (bestCount == needed_blocks) {
					break;
				}
			}
		}

		block += availableBlocks;
	}

	DEBUG_ASSERT(bestCount != 0);

	return bestBlock;
}

unsigned int BlockAllocator::Allocate(unsigned int size)
{
	unsigned int aligned_size = AlignUp(size, m_blockSize);
	unsigned int count        = aligned_size / m_blockSize;

	DEBUG_ASSERT(count > 0);

	unsigned int block = FindFreeBlocks(count + DBG_EXPAND) + DBG_PAD;

	for (unsigned int i = 0; i < count; i++) {
		MarkBlockUsed(block + i, true);
	}

	return block * m_blockSize;
}

void BlockAllocator::Free(unsigned int offset)
{
	DEBUG_ASSERT(offset % m_blockSize == 0);
	unsigned int block = offset / m_blockSize;

	unsigned int usedBlocks = CountBlocks(block, true);

	DEBUG_ASSERT(usedBlocks > 0);

	for(unsigned int i = 0; i < usedBlocks; i++) {
		MarkBlockUsed(block + i, false);
	}
}

size_t BlockAllocator::AllocatedSize(unsigned int offset)
{
	DEBUG_ASSERT(offset % m_blockSize == 0);
	unsigned int block = offset / m_blockSize;

	unsigned int  usedBlocks = CountBlocks(block, true);
	return usedBlocks * m_blockSize;
}
////////////////////////////////////////////////////////////////////////////////
}
