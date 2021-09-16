#include "pch.h"
#include "ThreadManager.h"
#include "RefCounting.h"
#include "Memory.h"
#include "Allocator.h"
#include "LockFreeStack.h"

class Data
{
public:
	SListEntry _entry;

	int32 _hp;
	int32 _mp;
};

int main()
{
	SListHeader header;
	InitializeHead(&header);

	Data* data = new Data();
	data->_hp = 10;
	data->_mp = 20;
	PushEntrySList(&header, (SListEntry*)data);

	Data* popData = (Data*)PopEntrySList(&header);
}