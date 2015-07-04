#ifndef __ALLOCATOR_H__
#define __ALLOCATOR_H__

const size_t DEFAULT_MAX_CAPACITY = 1024 * 1024 * 1024;
const size_t DEFAULT_MIN_CHUNK_SIZE = 64;
const size_t DEFAULT_PAGE_SIZE = 1024 * 4;

class Allocator
{
	struct Free
	{
		Free(Free * _next = NULL) : next(_next) {}
		Free * next;
	};

	struct ListOfFree
	{
		ListOfFree(size_t _size = 0) : size(_size), first(NULL) {}
		Free * first;
		size_t size;
	};

	typedef long long unsigned BlockHeader;

	size_t getBlockSize(BlockHeader * header)
	{
		return *header & (~1);
	}

	bool isBlockUsed(BlockHeader * header)
	{
		return *header & 1;
	}

	void setBlockUsage(BlockHeader * header, bool used)
	{
		*header = used ? *header | 1 : *header & ~1;
	}

	void setBlockSize(BlockHeader * header, long long unsigned size)
	{
		//size always is pow of 2 so we can ignore 1st bit
		//size &= ~1;

		//clear everything except usage
		*header &= 1;
		*header |= size;
	}

public:
	Allocator(size_t cap = DEFAULT_MAX_CAPACITY, size_t min = DEFAULT_MIN_CHUNK_SIZE);
	~Allocator();

	Allocator(const Allocator&) = delete;
	Allocator& operator= (const Allocator&) = delete;

	void * alloc(size_t bytesCnt);
	void free(void * address);

	size_t GetSize() { return m_RawMaxSize; }
	size_t GetMinChunkSize() { return m_MinBlockSize; }

private:
	BYTE * m_RawBuffer;
	size_t m_RawMaxSize;
	size_t m_MinBlockSize;
	std::vector<ListOfFree> m_FreeLists;

	void InitLists();
	void SetCapacity(size_t cap);
	void Split(size_t startIdx, size_t wantedIdx);
	void InitBlock(BYTE * buff, size_t size, size_t idxToPush);
	BYTE * Merge(BYTE * ptr);
};

#endif //__ALLOCATOR_H__