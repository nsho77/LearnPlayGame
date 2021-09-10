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
	// 1) �̹� ������� Ŭ���� ������� ��� �Ұ�
	// 2) ��ȯ(Cycle) ����

	// shared_ptr
	// weak_ptr
	// [Knight][RefCountingBlock](2���� ������ ���� ������)

	//[T*][RefCountingBlock]
	shared_ptr<Knight> spr(new Knight());

	//[T*][RefCountingBlock]
	shared_ptr<Knight> spr2 = spr;

	// shared_ptr<Knight> spr(new Knight());
	// �� make_shared<T>�� ������
	// [Knight | RefCountingBlock]
	shared_ptr<Knight> spr3 = make_shared<Knight>();

	// RefCountingBlock�� � ������ ������ ����?
	// uses(shared), weak(weak)

	weak_ptr<Knight> wpr = spr3;

	bool expired = wpr.expired();
	shared_ptr<Knight> spr4 = wpr.lock();
	if (spr4 != nullptr)
	{

	}

	// RefCountingBlock�� uses, weak��� 0�̾�� ������.

	// weakPtr�� ����ϸ� ��ȯ������ �ذ��� �� ����.


}