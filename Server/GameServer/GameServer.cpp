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
	//		v.clear(); // 이후부터 유효하지 않은데, 또 접근.. 메모리 오염
	//	}
	//}

	//Player* p = new Player();
	//Knight* k = static_cast<Knight*>(p); // casting 오류 ,, dynamic_cast를 사용하고 nullptr를 체크하면 되지만 느림.
	//k->_hp = 100; // 메모리 오염

	// 가상 메모리 기본
	// 다른 프로그램에서 address 참조 못함.. 운영체제가 만든 가상 주소임.
	//int* num2 = reinterpret_cast<int*>(12314214212);
	//*num2 = 200;

	// 유저레벨 (메모장, 롤, 서버)
	// --------------------------
	// 커널레벨 (OS Code)

	// 2GB [                      ]
	// 2GB [ooxxooxxoxooooxoxoooos]
	// 페이징 정책을 쓴다 4Kb[r][w][rw][][][][][] 페이지에 보안정책을 설정할 수 있다.

	//SYSTEM_INFO info;
	//::GetSystemInfo(&info);

	//info.dwPageSize; // 4KB (0x1000)
	//info.dwAllocationGranularity; // 64KB (0x10000) 메모리 할당할 때 이 숫자의 배수로 할당한다.

	//// 운영체제에 직접 메모리 할당, 반환 요청
	//Knight* test = (Knight*)::VirtualAlloc(NULL, 4, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
	//test->_hp = 100;
	//::VirtualFree(test, 0, MEM_RELEASE);
	//test->_hp = 200; // Crash가 발생한다.

	//delete num;

	/*Knight* knight = xnew<Knight>();

	xdelete(knight);

	knight->_hp = 100;*/

	// overflow 문제 잡기
	//[                         [    ]]
	Knight* knight = (Knight*)xnew<Player>();

	knight->_hp = 100;
	xdelete(knight);

}