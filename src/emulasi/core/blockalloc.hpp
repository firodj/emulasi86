#pragma once

#include <memory>

namespace emulasi
{
////////////////////////////////////////////////////////////////////////////////
class BlockAllocator
{
public:
	BlockAllocator(unsigned int size, unsigned int blockSize);
	~BlockAllocator();

	unsigned int Allocate(unsigned int size);
	void         Free(unsigned int offset);
	size_t       AllocatedSize(unsigned int offset);

protected:
	void         MarkBlockUsed(unsigned int block, bool used);
	bool         IsBlockUsed(unsigned int block);
	unsigned int CountBlocks(unsigned int block, bool used);
	unsigned int FindFreeBlocks(unsigned int needed_blocks);

	unsigned int               m_blockCount{};
	unsigned int               m_blockSize{};
	std::unique_ptr<uint8_t[]> m_blockUsage{};

};
////////////////////////////////////////////////////////////////////////////////
};
