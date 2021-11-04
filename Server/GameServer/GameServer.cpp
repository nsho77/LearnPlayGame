#include "pch.h"
#include "ThreadManager.h"
#include "RefCounting.h"
#include "Memory.h"
#include "Allocator.h"

#include <WinSock2.h>
#include <MSWSock.h>
#include <WS2tcpip.h>
#pragma comment(lib, "ws2_32.lib")

int main()
{
	// ���� �ʱ�ȭ (ws2_32 ���̺귯�� �ʱ�ȭ)
	// ���� ������ wsaData�� ä����
	WSAData wsaData;
	if (::WSAStartup(MAKEWORD(2, 2), &wsaData))
		return 0;

	// ad: Address Family (AF_INET = IPV4, AF_INET6 = IPv6)
	// type : TCP(SOCK_STREAM) vs UDP(SOCK_DGRAM)
	// protocol : 0
	// return : descriptor
	SOCKET listenSocket = ::socket(AF_INET, SOCK_STREAM, 0);
	if (listenSocket == INVALID_SOCKET)
	{
		int32 errCode = ::WSAGetLastError();
		cout << "Socket ErrorCode : " << errCode << endl;
		return 0;
	}

	// ���� �ּҴ�?
	SOCKADDR_IN serverAddr; // IPv4
	::memset(&serverAddr, 0, sizeof(serverAddr));
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_addr.s_addr = ::htonl(INADDR_ANY); // <�ϰ� �˾Ƽ� ����
	// �� �� Ư���ϸ� �ش� �ּҷθ� �����. 
	// ���� ��Ʈ��ũ ī�尡 ���� �� �־� �ּҸ� ���� �� ������ �ְ�
	// INADDR_ANY�ϸ� ���������� �ּҸ� ��� ������.
	serverAddr.sin_port = ::htons(7777);

	// �ȳ��� �� ����! �Ĵ��� ��ǥ ��ȣ
	if (::bind(listenSocket, (SOCKADDR*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR)
	{
		int32 errCode = ::WSAGetLastError();
		cout << "Socket ErrorCode : " << errCode << endl;
		return 0;
	}

	// ���� ����!
	// backlog : ��⿭�� max count
	if (::listen(listenSocket, 10) == SOCKET_ERROR)
	{
		int32 errCode = ::WSAGetLastError();
		cout << "Socket ErrorCode : " << errCode << endl;
		return 0;
	}

	// ----------------------------
	while (true)
	{
		SOCKADDR_IN clientAddr; // IPv4
		::memset(&clientAddr, 0, sizeof(clientAddr));
		int32 addrLen = sizeof(clientAddr);
		SOCKET clientSocket = ::accept(listenSocket, (SOCKADDR*)&clientAddr, &addrLen);
		if (clientSocket == INVALID_SOCKET)
		{
			int32 errCode = ::WSAGetLastError();
			cout << "Socket ErrorCode : " << errCode << endl;
			return 0;
		}

		// �մ� ����!
		char ipAddress[16];
		::inet_ntop(AF_INET, &clientAddr.sin_addr, ipAddress, sizeof(ipAddress));
		cout << "Client Connected! IP = " << ipAddress << endl;

		// TODO
	}

	// ----------------------------


	// ���� ����
	::WSACleanup();
}