#include "pch.h"
#include "ThreadManager.h"
#include "RefCounting.h"
#include "Memory.h"
#include "Allocator.h"

class Knight
{
public:
	int32 _hp = rand() % 1000;
};

class Monster
{
public:
	int64 _id = 0;
};

int main()
{
	shared_ptr<Knight> k = ObjectPool<Knight>::MakeShared();
	shared_ptr<Knight> k2 = MakeShared<Knight>();

	for (int32 i = 0; i < 5; i++)
	{
		GThreadManager->Launch([]()
			{
				while (true)
				{
					Knight* knight = xnew<Knight>();
					cout << knight->_hp << endl;

					this_thread::sleep_for(10ms);

					xdelete(knight);
				}
			});
	}

	GThreadManager->Join();
}