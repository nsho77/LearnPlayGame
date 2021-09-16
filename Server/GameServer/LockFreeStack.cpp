#include "pch.h"
#include "LockFreeStack.h"

/*----------------
	  1차 시도
----------------*/
/*
// [data][  ][  ]
// Header[next]

void InitializeHead(SListHeader* header)
{
	header->next = nullptr;
}

void PushEntrySList(SListHeader* header, SListEntry* entry)
{
	entry->next = header->next;
	header->next = entry;
}

SListEntry* PopEntrySList(SListHeader* header)
{
	SListEntry* first = header->next;

	if (first != nullptr)
		header->next = first->next;

	return first;
}
*/

/*----------------
	  2차 시도
----------------*/

void InitializeHead(SListHeader* header)
{
	header->next = nullptr;
}

void PushEntrySList(SListHeader* header, SListEntry* entry)
{
	entry->next = header->next;
	while (::InterlockedCompareExchange64((int64*)&header->next, (int64)entry, (int64)entry->next) == 0)
	{

	}
}

SListEntry* PopEntrySList(SListHeader* header)
{
	SListEntry* expected = header->next;
	
	// ABA Problem

	// [5000]->[7000]

	// 만약에 Header가 5000이라면, Header에다 6000을 넣어줘!
	// [5000]->[6000]->[7000]
	// [Header]
	while (expected && ::InterlockedCompareExchange64((int64*)&header->next, (int64)expected->next, (int64)expected) == 0)
	{

	}

	return expected;
}