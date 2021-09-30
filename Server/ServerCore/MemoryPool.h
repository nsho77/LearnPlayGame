#pragma once

enum
{
	SLIST_ALIGNMENT = 16
};

/*---------------------
	MemoryHeader
---------------------*/
DECLSPEC_ALIGN(SLIST_ALIGNMENT)
struct MemoryHeader : public SLIST_ENTRY
{
	// [MemoryHeader][Data] : 표준 new, delete도 같은 방식

	MemoryHeader(int32 size) : allocSize(size) {}

	static void* AttchHeader(MemoryHeader* header, int32 size)
	{
		new(header)MemoryHeader(size); // placement new
		return reinterpret_cast<void*>(++header);
	}

	static MemoryHeader* DetachHeader(void* ptr)
	{
		MemoryHeader* header = reinterpret_cast<MemoryHeader*>(ptr) - 1;
		return header;
	}

	int32 allocSize;
};

/*---------------------
	MemoryPool
---------------------*/

DECLSPEC_ALIGN(SLIST_ALIGNMENT)
class MemoryPool
{
public:
	MemoryPool(int32 allocSize);
	~MemoryPool();

	void Push(MemoryHeader* ptr);
	MemoryHeader* Pop();

private:
	SLIST_HEADER _header;
	int32 _allocSize = 0;
	atomic<int32> _allocCount = 0;
};

