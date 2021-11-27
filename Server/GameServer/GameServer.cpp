#include "pch.h"
#include "ThreadManager.h"
#include "RefCounting.h"
#include "Memory.h"
#include "Allocator.h"

#include "SocketUtils.h"

int main()
{
	SOCKET socket = SocketUtils::CreateSocket();

	SocketUtils::BindAnyAddress(socket, 7777);

	SocketUtils::Listen(socket);

	SOCKET clientSocket = ::accept(socket, nullptr, nullptr);

	cout << "Client Connected!" << endl;

	GThreadManager->Join();
}