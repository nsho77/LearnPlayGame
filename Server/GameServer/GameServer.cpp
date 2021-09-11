#include "pch.h"
#include "ThreadManager.h"
#include "RefCounting.h"
#include "Memory.h"

class Player
{

};

class Knight : public Player
{
public:
	Knight()
	{
		cout << "Knight()" << endl;
	}

	Knight(int32 hp) : _hp(hp)
	{
		cout << "Knight(hp)" << endl;
	}

	~Knight()
	{
		cout << "~Knight()" << endl;
	}

	int32 _hp = 100;
	int32 _mp = false;
};


int main()
{
	//Knight* k1 = new Knight();
	//k1->_hp = 200;
	//k1->_mp = 50;
	//delete k1;
	//k1->_hp = 100; // User-After-Free

	//vector<int32> v{ 1,2,3,4,5 };
	//for (int32 i = 0; i < 5; i++)
	//{
	//	int32 value = v[i];
	//	// TODO
	//	if (value == 3)
	//	{
	//		v.clear(); // ���ĺ��� ��ȿ���� ������, �� ����.. �޸� ����
	//	}
	//}

	//Player* p = new Player();
	//Knight* k = static_cast<Knight*>(p); // casting ���� ,, dynamic_cast�� ����ϰ� nullptr�� üũ�ϸ� ������ ����.
	//k->_hp = 100; // �޸� ����

	// ���� �޸� �⺻
	// �ٸ� ���α׷����� address ���� ����.. �ü���� ���� ���� �ּ���.
	//int* num2 = reinterpret_cast<int*>(12314214212);
	//*num2 = 200;

	// �������� (�޸���, ��, ����)
	// --------------------------
	// Ŀ�η��� (OS Code)

	// 2GB [                      ]
	// 2GB [ooxxooxxoxooooxoxoooos]
	// ����¡ ��å�� ���� 4Kb[r][w][rw][][][][][] �������� ������å�� ������ �� �ִ�.

	//SYSTEM_INFO info;
	//::GetSystemInfo(&info);

	//info.dwPageSize; // 4KB (0x1000)
	//info.dwAllocationGranularity; // 64KB (0x10000) �޸� �Ҵ��� �� �� ������ ����� �Ҵ��Ѵ�.

	//// �ü���� ���� �޸� �Ҵ�, ��ȯ ��û
	//Knight* test = (Knight*)::VirtualAlloc(NULL, 4, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
	//test->_hp = 100;
	//::VirtualFree(test, 0, MEM_RELEASE);
	//test->_hp = 200; // Crash�� �߻��Ѵ�.

	//delete num;

	/*Knight* knight = xnew<Knight>();

	xdelete(knight);

	knight->_hp = 100;*/

	// overflow ���� ���
	//[                         [    ]]
	Knight* knight = (Knight*)xnew<Player>();

	knight->_hp = 100;
	xdelete(knight);

}