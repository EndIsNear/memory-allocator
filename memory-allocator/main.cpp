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
		SECTION("Bad allocs")
		{
			Allocator alloc(1024, 16);
			CHECK(alloc.alloc(0) == NULL);
			CHECK(alloc.alloc(1025) == NULL);
			CHECK(alloc.alloc(-1) == NULL);
		}

		SECTION("The whole size")
		{
			Allocator alloc(1024, 16);
			char * res = static_cast<char*>(alloc.alloc(1016));
			REQUIRE(res != NULL);

			//just use it for potential errors
			for (int i = 0; i < 1016; ++i)
			{
				res[i]++;
			}
			res = static_cast<char*>(alloc.alloc(1));
			CHECK(res == NULL);
			res = static_cast<char*>(alloc.alloc(504));
			CHECK(res == NULL);
			res = static_cast<char*>(alloc.alloc(1016));
			CHECK(res == NULL);
		}

		SECTION("Always the half of free mem")
		{
			Allocator alloc(1024, 16);
			char * res;
			for (int i = 512; i >= 16; i /= 2)
			{
				res = static_cast<char*>(alloc.alloc(i - 8));
				REQUIRE(res != NULL);
				for (int i = 0; i < i - 8; ++i)
				{
					res[i]++;
				}
			}
			res = static_cast<char*>(alloc.alloc(1));
			CHECK(res != NULL);
			res = static_cast<char*>(alloc.alloc(1));
			CHECK(res == NULL);
			res = static_cast<char*>(alloc.alloc(504));
			CHECK(res == NULL);
			res = static_cast<char*>(alloc.alloc(1016));
			CHECK(res == NULL);
		}

		SECTION("Max size in 2 passes")
		{
			Allocator alloc(1024, 16);
			char * res;
			for (int j = 0; j < 2; ++j)
			{
				res = static_cast<char*>(alloc.alloc(504));
				REQUIRE(res != NULL);

				//just use it for potential errors
				for (int i = 0; i < 504; ++i)
				{
					res[i]++;
				}
			}

			res = static_cast<char*>(alloc.alloc(8));
			CHECK(res == NULL);
			res = static_cast<char*>(alloc.alloc(504));
			CHECK(res == NULL);
			res = static_cast<char*>(alloc.alloc(1016));
			CHECK(res == NULL);
		}

		SECTION("Max size in 2 passes")
		{
			Allocator alloc(1024, 16);
			char * res;
			for (int j = 0; j < 64; ++j)
			{
				res = static_cast<char*>(alloc.alloc(8));
				REQUIRE(res != NULL);

				for (int i = 0; i < 8; ++i)
				{
					res[i]++;
				}
			}

			res = static_cast<char*>(alloc.alloc(1));
			CHECK(res == NULL);
			res = static_cast<char*>(alloc.alloc(504));
			CHECK(res == NULL);
			res = static_cast<char*>(alloc.alloc(1016));
			CHECK(res == NULL);
		}
	}

	SECTION("alloc free alloc")
	{
		SECTION("one - one")
		{
			Allocator alloc(1024, 16);
			char * res = static_cast<char*>(alloc.alloc(1016));
			CHECK(res != NULL);
			alloc.free(res);
			res = NULL;
			res = static_cast<char*>(alloc.alloc(1016));
			CHECK(res != NULL);
		}

		SECTION("one small - biggest")
		{
			Allocator alloc(1024, 16);
			char * res = static_cast<char*>(alloc.alloc(8));
			CHECK(res != NULL);
			alloc.free(res);
			res = NULL;
			res = static_cast<char*>(alloc.alloc(1016));
			CHECK(res != NULL);
		}

		SECTION("two - one")
		{
			Allocator alloc(1024, 16);
			char * res1 = static_cast<char*>(alloc.alloc(504));
			char * res2 = static_cast<char*>(alloc.alloc(504));
			CHECK(res1 != NULL);
			CHECK(res2 != NULL);
			alloc.free(res1);
			alloc.free(res2);
			res1 = NULL;
			res1 = static_cast<char*>(alloc.alloc(1016));
			CHECK(res1 != NULL);
		}

		SECTION("two - two")
		{
			Allocator alloc(1024, 16);
			char * res1 = static_cast<char*>(alloc.alloc(504));
			char * res2 = static_cast<char*>(alloc.alloc(504));
			CHECK(res1 != NULL);
			CHECK(res2 != NULL);
			alloc.free(res1);
			alloc.free(res2);
			res1 = NULL;
			res1 = static_cast<char*>(alloc.alloc(504));
			res2 = NULL;
			res2 = static_cast<char*>(alloc.alloc(504));
			CHECK(res1 != NULL);
			CHECK(res2 != NULL);
		}
	}
}