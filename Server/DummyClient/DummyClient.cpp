#include "pch.h"
#include <iostream>

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
	SOCKET clientSocket = ::socket(AF_INET, SOCK_STREAM, 0);
	if (clientSocket == INVALID_SOCKET)
	{
		int32 errCode = ::WSAGetLastError();
		cout << "Socket ErrorCode : " << errCode << endl;
		return 0;
	}

	SOCKADDR_IN serverAddr; // IPv4
	::memset(&serverAddr, 0, sizeof(serverAddr));
	serverAddr.sin_family = AF_INET;
	//serverAddr.sin_addr.s_addr = ::inet_addr("127.0.0.1");
	::inet_pton(AF_INET, "127.0.0.1", &serverAddr.sin_addr);
	serverAddr.sin_port = ::htons(7777);

	// host to network short(ȣ��Ʈ���� ��Ʈ��ũ ��� ��������� �ٲٰڴ�)
	// Little-Endian vs Big-Endian
	// ex) 0x12345678 4����Ʈ ����
	// low [0x78][0x56][0x34][0x12] high > little
	// low [0x12][0x34][0x56][0x78] hight > big = networkǥ��
	// Ȯ��
	// int16 num = 0x1234;
	// int16 num2 = ::htons(num);

	if (::connect(clientSocket, (SOCKADDR*)&serverAddr, sizeof(serverAddr)))
	{
		int32 errCode = ::WSAGetLastError();
		cout << "Socket ErrorCode : " << errCode << endl;
		return 0;
	}

	// ----------------------------
	// ���� ����! �������� ������ �ۼ��� ����!

	cout << "Connected to Server!" << endl;
	while (true)
	{
		// TODO

		this_thread::sleep_for(1s);
	}

	// ---------------------------

	// ���� ���ҽ� ��ȯ
	::closesocket(clientSocket);

	// ���� ����
	::WSACleanup();
}
