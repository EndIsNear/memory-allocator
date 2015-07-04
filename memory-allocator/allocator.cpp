#include <exception>
#include <vector>

#include "allocator.h"

Allocator::Allocator(size_t cap, size_t min)
{
	SetCapacity(cap);
	m_MinChunkSize = (min < 16 || (min & (min - 1))) ? 16 : min;

	m_RawBuffer = static_cast<BYTE*>(malloc(m_RawMaxSize));
	if (!m_RawBuffer)
		throw std::exception("Not enought memory");

	InitLists();
}

Allocator::~Allocator()
{
	free(m_RawBuffer);
}

void * Allocator::alloc(size_t size)
{
	if (size > m_RawMaxSize - sizeof(BlockHeader) || size == 0)
		return NULL;

	size_t idx = 0;
	while (idx + 1 < m_FreeLists.size() && m_FreeLists[idx + 1].size >= size)
		idx++;

	if (!m_FreeLists[idx].first)
	{
		size_t wantedIdx = idx;
		while (idx > 0 && !m_FreeLists[idx].first)
			idx--;
		//out of memory
		if (idx == 0 && !m_FreeLists[idx].first)
		{
			//throw std::exception("Out of memory");
			return NULL;
		}

		Split(idx, wantedIdx);
		idx = wantedIdx;
	}

	BYTE * res = reinterpret_cast<BYTE*>(m_FreeLists[idx].first);
	m_FreeLists[idx].first = m_FreeLists[idx].first->next;
	//header size is ok just make it as used
	setBlockUsage(reinterpret_cast<BlockHeader*>(res - 8), true);
	return static_cast<void*>(res);
}

void Allocator::free(void * ptr)
{

}

void Allocator::Split(size_t startIdx, size_t wantedIdx)
{
	BYTE * start = reinterpret_cast<BYTE*>(m_FreeLists[startIdx].first) - sizeof(BlockHeader);
	m_FreeLists[startIdx].first = m_FreeLists[startIdx].first->next;

	size_t size = getBlockSize(reinterpret_cast<BlockHeader*>(start));
	InitBlock(start, size / 2, startIdx + 1);
	InitBlock(start + size/2, size / 2, startIdx + 1);
	if (startIdx + 1 < wantedIdx)
		Split(startIdx + 1, wantedIdx);
}

void Allocator::InitLists()
{
	size_t crnSize = m_RawMaxSize;
	while (crnSize > m_MinChunkSize)
	{
		m_FreeLists.push_back(ListOfFree(crnSize - sizeof(BlockHeader)));
		crnSize >>= 1;
	}
	InitBlock(m_RawBuffer, m_RawMaxSize, 0);
}

void Allocator::InitBlock(BYTE * buff, size_t size, size_t idxToPush)
{
	setBlockSize(reinterpret_cast<BlockHeader*>(buff), size);
	setBlockUsage(reinterpret_cast<BlockHeader*>(buff), false);
	reinterpret_cast<Free*>(buff + sizeof(BlockHeader))->next = m_FreeLists[idxToPush].first;
	m_FreeLists[idxToPush].first = reinterpret_cast<Free*>(buff + sizeof(BlockHeader));
}

void Allocator::SetCapacity(size_t cap)
{
	if (cap < 1024)
		m_RawMaxSize = 1024;
	else if (cap & (cap - 1))
	{
		size_t c = 1;
		while (c < cap)
			c <<= 1;
		m_RawMaxSize = c >> 1;
	}
	else
		m_RawMaxSize = cap;
}
