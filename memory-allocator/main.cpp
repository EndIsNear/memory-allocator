#define CATCH_CONFIG_MAIN

#include <iostream>
#include "catch.hpp"

#include "allocator.h"

TEST_CASE("MemoryAllocator")
{
	SECTION("Ctor")
	{
		SECTION("Default")
		{
			Allocator alloc;
			CHECK(alloc.GetSize() == DEFAULT_MAX_CAPACITY);
			CHECK(alloc.GetMinChunkSize() == DEFAULT_MIN_CHUNK_SIZE);
		}

		SECTION("pow of 2 vals")
		{
			Allocator alloc(1024, 16);
			CHECK(alloc.GetSize() == 1024);
			CHECK(alloc.GetMinChunkSize() == 16);
		}

		SECTION("wrong vals")
		{
			Allocator alloc(2048, 3);
			CHECK(alloc.GetSize() == 2048);
			CHECK(alloc.GetMinChunkSize() == 16);
		}

		SECTION("wrong vals2")
		{
			Allocator alloc(900, 32);
			CHECK(alloc.GetSize() == 1024);
			CHECK(alloc.GetMinChunkSize() == 32);
		}

		SECTION("wrong vals3")
		{
			Allocator alloc(0, 0);
			CHECK(alloc.GetSize() == 1024);
			CHECK(alloc.GetMinChunkSize() == 16);
		}
	}

	SECTION("Alloc")
	{

	}
}