#include "pch.h"
#include "ThreadManager.h"
#include "RefCounting.h"
#include "Memory.h"
#include "Allocator.h"

#include <WinSock2.h>
#include <MSWSock.h>
#include <WS2tcpip.h>
#pragma comment(lib, "ws2_32.lib")

void HandleError(const char* cause)
{
	int32 errCode = ::WSAGetLastError();
	cout << cause << " ErrorCode : " << errCode << endl;
}

const int32 BUFSIZE = 1000;

struct Session
{
	WSAOVERLAPPED overlapped = {};
	SOCKET socket;
	char recvBuffer[BUFSIZE] = {};
	int32 recvBytes = 0;
};

void CALLBACK RecvCallback(DWORD error, DWORD recvLen, LPWSAOVERLAPPED overlapped, DWORD flags)
{
	cout << "Data Recv Len Callback = " << recvLen << endl;
	// TODO : 에코 서버를 만든다면 WSASend()

	// param 중 유용한 정보는 overlapped.
	// 구조체를 변형하여 원하는 구조체로 casting하여 사용한다.
	Session* session = (Session*)overlapped;
}

int main()
{
	WSAData wsaData;
	if (::WSAStartup(MAKEWORD(2, 2), &wsaData))
		return 0;

	// 블록킹(Blocking)소켓
	// accept -> 접속한 클라가 있을 때
	// connect -> 서버 접속 성공했을 때
	// send, sendto -> 요청한 데이터를 송신 버퍼에 복사했을 때
	// recv, recvfrom -> 수신 버퍼에 도착한 데이터가 있고, 이를 유저레벨 버퍼에 복사했을 때

	// 논블록킹(Non-Blocking)

	SOCKET listenSocket = ::socket(AF_INET, SOCK_STREAM, 0);
	if (listenSocket == INVALID_SOCKET)
		return 0;

	u_long on = 1;
	if (::ioctlsocket(listenSocket, FIONBIO, &on) == INVALID_SOCKET)
		return 0;

	SOCKADDR_IN serverAddr;
	::memset(&serverAddr, 0, sizeof(serverAddr));
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_addr.s_addr = ::htonl(INADDR_ANY);
	serverAddr.sin_port = ::htons(7777);

	if (::bind(listenSocket, (SOCKADDR*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR)
		return 0;

	if (::listen(listenSocket, SOMAXCONN) == SOCKET_ERROR)
		return 0;
	
	cout << "Accept" << endl;

	// Overlapped IO (비동기 + 논블록킹)
	// - Overlapped 함수를 건다 (WSARecv, WSASend)
	// - Overlapped 함수가 성공했는지 확인 후,
	// -> 성공했으면 결과 얻어서 처리
	// -> 실패했으면 사유를 확인

	// 1) 비동기 입출력 소켓
	// 2) WSABUf 배열의 시작 주소 + 개수 // Scatter-Gather
	//    char sendBuffer[100];
	//    WSABUF wsaBuf[2];
	//    wsaBuf[0].buf = sendBuffer;
	//    wsaBuf[0].len = 100;
	//    char sendBuffer2[100];
	//    wsaBuf[1].buf = sendBuffer2;
	//    wsaBuf[1].len = 100;
	// 3) 보내고/받은 바이트 수
	// 4) 상세 옵션 0
	// 5) WSAOVERLAPPED 구조체 주소값
	// 6) 입출력이 완료되면 OS가 호출할 콜백 함수
	// WSASend
	// WSARecv

	// Overlapped 모델(Completion Routine 콜백 기반)
	// - 비동기 입출력 지원하는 소켓 생성
	// - 비동기 입출력 함수 호출(완료 루틴의 시작 주소를 넘겨준다)
	// - 비동기 작업이 바로 완료되지 않으면, WSA_IO_PENDING 오류 코드
	// - 비동기 입출력 함수 호출한 쓰레드를 -> Alertable Wait 상태로 만든다
	// ex) WaitForSingleObjectEx, WaitForMultipleObjectsEx, SleepEx, WSAWaitForMultipleEvents
	// - 비동기 IO 완료되면, 운영체제는 완료 루틴 호출
	// - 완료 루틴 호출이 모두 끝나면, 쓰레드는 Alertable Wait 상태에서 빠져나온다.

	// 1) 오류 발생시 0 아닌 값
	// 2) 전송 바이트 수
	// 3) 비동기 입출력 함수 호출 시 넘겨준 WSAOVERLAPPED 구조체의 주소값
	// 4) 0
	// void CompletionRoutine()

	// Select 모델
	// - 장점) 윈도우/리눅스 공통.
	// - 단점) 성능 최하 (매번 등록 비용), 64개 제한
	// WSAEventSelect 모델
	// - 장점) 비교적 뛰어난 성능
	// - 단점) 64개 제한
	// Overlapped (이벤트 기반)
	// - 장점) 성능
	// - 단점) 64개 제한
	// Overlapped (콜백 기반)
	// - 장점) 성능
	// - 단점) 모든 비동기 소켓 함수에서 사용 가능하진 않음(accept). 빈번한 Alertable Wait으로 인한 성능 저하

	// pattern 용어
	// Reactor Pattern (~뒤늦게. 논블록킹 소켓. 소켓 상태 확인 후 -> 뒤늦게 recv send 호출)
	// Proactor Pattern (~미리, Overlapped WSA~)

	while (true)
	{
		SOCKADDR_IN clientAddr;
		int32 addrLen = sizeof(clientAddr);

		SOCKET clientSocket;
		while (true)
		{
			clientSocket = ::accept(listenSocket, (SOCKADDR*)&clientAddr, &addrLen);
			if (clientSocket != INVALID_SOCKET)
				break;

			if (::WSAGetLastError() == WSAEWOULDBLOCK)
				continue;

			// 문제 있는 상황
			return 0;
		}

		Session session = Session{ clientSocket };
		//WSAEVENT wsaEvent = ::WSACreateEvent();

		cout << "Client Connected !" << endl;

		while (true)
		{
			WSABUF wsaBuf;
			wsaBuf.buf = session.recvBuffer;
			wsaBuf.len = BUFSIZE;

			DWORD recvLen = 0;
			DWORD flags = 0;
			if (::WSARecv(clientSocket, &wsaBuf, 1, &recvLen, &flags, &session.overlapped, RecvCallback) == SOCKET_ERROR)
			{
				if (::WSAGetLastError() == WSA_IO_PENDING)
				{
					// Pending
					// Alertable Wait
					// alertable wait apc 검색해보기
					// 코드 진행
					// - alertable wait 상태 > apc 큐 모두 비우기 > alertable wait 해줬던 이후부터 다시 진행
					::SleepEx(INFINITE, TRUE);
					// 아래 방식은 기다릴 수 있는 event 개수가 64개로 정해져 있어 불편
					// socket과 event를 1:1로 매핑해줘야 해서 불편
					//::WSAWaitForMultipleEvents(1, &wsaEvent, TRUE, WSA_INFINITE, TRUE);
					
				}
				else
				{
					// TODO : 문제 있는 상황
					break;
				}
			}

			cout << "Data Recv Len = " << recvLen << endl;
		}

		::closesocket(session.socket);
		//::WSACloseEvent(wsaEvent);
	}

	// 윈속 종료
	::WSACleanup();
}