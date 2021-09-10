#include "pch.h"
#include "ThreadManager.h"
#include "RefCounting.h"

using InventoryRef = TSharedPtr<class Inventory>;

class Knight
{
public:
	Knight()
	{
		cout << "Knight()" << endl;
	}

	~Knight()
	{
		cout << "~Knight()" << endl;
	}
};


int main()
{
	// 1) 이미 만들어진 클래스 대상으로 사용 불가
	// 2) 순환(Cycle) 문제

	// shared_ptr
	// weak_ptr
	// [Knight][RefCountingBlock](2개의 정보를 따로 관리함)

	//[T*][RefCountingBlock]
	shared_ptr<Knight> spr(new Knight());

	//[T*][RefCountingBlock]
	shared_ptr<Knight> spr2 = spr;

	// shared_ptr<Knight> spr(new Knight());
	// 와 make_shared<T>의 차이점
	// [Knight | RefCountingBlock]
	shared_ptr<Knight> spr3 = make_shared<Knight>();

	// RefCountingBlock은 어떤 정보를 가지고 있음?
	// uses(shared), weak(weak)

	weak_ptr<Knight> wpr = spr3;

	bool expired = wpr.expired();
	shared_ptr<Knight> spr4 = wpr.lock();
	if (spr4 != nullptr)
	{

	}

	// RefCountingBlock은 uses, weak모두 0이어야 해제됨.

	// weakPtr을 사용하면 순환참조를 해결할 수 있음.


}