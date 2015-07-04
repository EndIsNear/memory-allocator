#include <exception>
#include <vector>

#include "allocator.h"

Allocator::Allocator(size_t cap, size_t min)
{
	SetCapacity(cap);
	m_MinChunkSize = (min < 16 || (min & (min - 1))) ? 16 : min;

	m_RawBuffer = malloc(m_RawMaxSize);
	if (!m_RawBuffer)
		throw std::exception("Not enought memory");

	InitLists();
	static_cast<Free*>(m_RawBuffer)->next = NULL;
	m_FreeLists[0].first = static_cast<Free*>(m_RawBuffer);
}

Allocator::~Allocator()
{
	free(m_RawBuffer);
}


void Allocator::InitLists()
{
	size_t crnSize = m_RawMaxSize;
	while (crnSize >= m_MinChunkSize)
	{
		m_FreeLists.push_back(ListOfFree(crnSize));
		crnSize <<= 1;
	}
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
