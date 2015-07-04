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
public:
	Allocator(size_t cap = DEFAULT_MAX_CAPACITY, size_t min = DEFAULT_MIN_CHUNK_SIZE);
	~Allocator();

	Allocator(const Allocator&) = delete;
	Allocator& operator= (const Allocator&) = delete;

	void * alloc(size_t bytesCnt){};
	void free(void * address){};

	size_t GetSize() { return m_RawMaxSize; }
	size_t GetMinChunkSize() { return m_MinChunkSize; }

private:
	void * m_RawBuffer;
	size_t m_RawMaxSize;
	size_t m_MinChunkSize;
	std::vector<ListOfFree> m_FreeLists;

	void InitLists();
	void SetCapacity(size_t cap);
};

#endif //__ALLOCATOR_H__