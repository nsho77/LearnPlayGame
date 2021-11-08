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

int main()
{
	WSAData wsaData;
	if (::WSAStartup(MAKEWORD(2, 2), &wsaData))
		return 0;

	SOCKET serverSocket = ::socket(AF_INET, SOCK_DGRAM, 0);
	if (serverSocket == INVALID_SOCKET)
	{
		HandleError("Socket");
		return 0;
	}

	// level : �ɼ��� �ؼ��ϰ� ó���� ��ü
	// ���� �ڵ� -> SOL_SOCKET
	// IPv4 -> IPPROTO_IP
	// TCP �������� -> IPPROTO_TCP

	// SO_KEEPALIVE = �ֱ������� ���� ���� Ȯ�� ���� (TCP Only)
	// ������ �Ҹ��ҹ� ���� ���� ���´ٸ�?
	// �ֱ������� TCP �������� ���� ���� Ȯ�� -> ������ ���� ����
	
	bool enable = true;
	::setsockopt(serverSocket, SOL_SOCKET, SO_KEEPALIVE, (char*)&enable, sizeof(enable));

	// SO_LINGER = �����ϴ�
	// �۽� ���ۿ� �ִ� �����͸� ���� ���ΰ�? ���� ���ΰ�?
	// onoff = 0�̸� closesocket()�� �ٷ� ����. �ƴϸ� linger�ʸ�ŭ ��� (default 0)
	// linger : ��� �ð�
	LINGER linger;
	linger.l_onoff = 1;
	linger.l_linger = 5;
	::setsockopt(serverSocket, SOL_SOCKET, SO_LINGER, (char*)&linger, sizeof(linger));

	// Half-Close
	// SD_SEND : send ���´�
	// SD_RECEIVE : recv ���´�
	// SD_BOTH : �Ѵ� ���´�
	//::shutdown(serverSocket, SD_SEND);

	// ���� ���ҽ� ��ȯ
	// send -> closesocket
	//::closesocket(serverSocket);

	// SO_SNDBUF = �۽� ���� ũ��
	// SO_RCVBUF = ���� ���� ũ��

	int32 sendBufferSize;
	int32 optionLen = sizeof(sendBufferSize);
	::getsockopt(serverSocket, SOL_SOCKET, SO_SNDBUF, (char*)&sendBufferSize, &optionLen);
	cout << "�۽� ���� ũ�� : " << sendBufferSize << endl;

	int32 recvBufferSize;
	optionLen = sizeof(recvBufferSize);
	::getsockopt(serverSocket, SOL_SOCKET, SO_RCVBUF, (char*)&recvBufferSize, &optionLen);
	cout << "���� ���� ũ�� : " << recvBufferSize << endl;

	// SO_REUSEADDR
	// IP�ּ� �� port ����
	{
		bool enable = true;
		::setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, (char*)&enable, sizeof(enable));
	}

	// IPPROT TCP
	// TCP_NODELAY = Nagle ���̱� �˰��� �۵� ����
	// �����Ͱ� ����� ũ�� ������, �׷��� ������ �����Ͱ� ����� ���϶����� ���!
	// ���� : ���� ��Ŷ�� ���ʿ��ϰ� ���� �����Ǵ� ���� ����
	// ���� : ���� �ð� ����
	{
		bool enable = true;
		::setsockopt(serverSocket, IPPROTO_TCP, TCP_NODELAY, (char*)&enable, sizeof(enable));
	}



	// ���� ����
	::WSACleanup();
}