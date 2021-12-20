#include "pch.h"
#include "Session.h"
#include "SocketUtils.h"
#include "Service.h"

/*----------------
  Session
----------------*/

Session::Session() : _recvBuffer(BUFFER_SIZE)
{
    _socket = SocketUtils::CreateSocket();
}

Session::~Session()
{
    SocketUtils::Close(_socket);
}

void Session::Send(BYTE* buffer, int32 len)
{
    // 생각할 문제
    // 1) 버퍼 관리?
    // 2) sendEvent 관리? 단일? 여러개? WSASend 중첩?

    // TEMP
    SendEvent* sendEvent = xnew<SendEvent>();
    sendEvent->owner = shared_from_this(); // ADD_REF
    sendEvent->buffer.resize(len);
    ::memcpy(sendEvent->buffer.data(), buffer, len);

    WRITE_LOCK; //WSASend는 thread-safe 하지 않다
    RegisterSend(sendEvent);
}

bool Session::Connect()
{
    return RegisterConnect();
}

void Session::Disconnect(const WCHAR* cause)
{
    if (_connected.exchange(false) == false)
        return;

    // TEMP
    wcout << "Disconnect : " << cause << endl;

    OnDisconnected(); // 컨텐츠 코드에서 오버라이딩
    GetService()->ReleaseSession(GetSessionRef());

    RegisterDisconnect();
}

HANDLE Session::GetHandle()
{
    return reinterpret_cast<HANDLE>(_socket);
}

void Session::Dispatch(IocpEvent* iocpEvent, int32 numOfBytes)
{
    switch (iocpEvent->eventType)
    {
    case EventType::Connect:
        ProcessConnect();
        break;
    case EventType::Disconnect:
        ProcessDisconnect();
        break;
    case EventType::Recv:
        ProcessRecv(numOfBytes);
        break;
    case EventType::Send:
        ProcessSend(static_cast<SendEvent*>(iocpEvent), numOfBytes);
        break;
    }
}

bool Session::RegisterConnect()
{
    if (IsConnected())
        return false;

    if (GetService()->GetServiceType() != ServiceType::Client)
        return false;

    if (SocketUtils::SetReuseAddress(_socket, true) == false)
        return false;

    if (SocketUtils::BindAnyAddress(_socket, 0/*남는거*/) == false)
        return false;

    _connectEvent.Init();
    _connectEvent.owner = shared_from_this(); // ADD_REF

    DWORD numOfBytes = 0;
    SOCKADDR_IN sockAddr = GetService()->GetNetAddress().GetSockAddr();
    if (false == SocketUtils::ConnectEx(_socket, reinterpret_cast<SOCKADDR*>(&sockAddr), sizeof(sockAddr), nullptr, 0, &numOfBytes, &_connectEvent))
    {
        int32 errorCode = ::WSAGetLastError();
        if (errorCode != WSA_IO_PENDING)
        {
            _connectEvent.owner = nullptr; // RELEASE_REF
            return false;
        }
    }

    return true;
}

bool Session::RegisterDisconnect()
{
    _disconnectEvent.Init();
    _disconnectEvent.owner = shared_from_this(); // ADD_REF

    if (false == SocketUtils::DisconnectEx(_socket, &_disconnectEvent, TF_REUSE_SOCKET, 0))
    {
        int32 errorCode = ::WSAGetLastError();
        if (errorCode != WSA_IO_PENDING)
        {
            _disconnectEvent.owner = nullptr; // RELEASE_REF
            return false;
        }
    }

    return false;
}

void Session::RegisterRecv()
{
    if (IsConnected() == false)
        return;

    _recvEvent.Init();
    _recvEvent.owner = shared_from_this(); // ADD_REF

    WSABUF wsaBuf;
    wsaBuf.buf = reinterpret_cast<char*>(_recvBuffer.WritePos());
    wsaBuf.len = _recvBuffer.FreeSize();

    DWORD numOfBytes = 0;
    DWORD flags = 0;
    if (SOCKET_ERROR == ::WSARecv(_socket, &wsaBuf, 1, OUT &numOfBytes, OUT &flags, &_recvEvent, nullptr))
    {
        int32 errorCode = ::WSAGetLastError();
        if (errorCode != WSA_IO_PENDING)
        {
            HandleError(errorCode);
            _recvEvent.owner = nullptr; // RELEASE_REF
        }
    }
}

void Session::RegisterSend(SendEvent* sendEvent)
{
    if (IsConnected() == false)
        return;

    WSABUF wsaBuf;
    wsaBuf.buf = (char*)sendEvent->buffer.data();
    wsaBuf.len = (ULONG)sendEvent->buffer.size();

    DWORD numOfBytes = 0;
    if (SOCKET_ERROR == ::WSASend(_socket, &wsaBuf, 1, OUT & numOfBytes, 0, sendEvent, nullptr))
    {
        int32 errorCode = ::WSAGetLastError();
        if (errorCode != WSA_IO_PENDING)
        {
            HandleError(errorCode);
            sendEvent->owner = nullptr; //RELEASE_REF
            xdelete(sendEvent);
        }
    }
}

void Session::ProcessConnect()
{
    _connectEvent.owner = nullptr; // RELEASE_REF

    _connected.store(true);

    // 세션 등록
    GetService()->AddSession(GetSessionRef());

    // 컨텐츠 코드에서 오버라이딩
    OnConnected();

    // 수신 등록
    RegisterRecv();
}

void Session::ProcessDisconnect()
{
    _disconnectEvent.owner = nullptr; // RELEASE_REF
}

void Session::ProcessRecv(int32 numOfBytes)
{
    _recvEvent.owner = nullptr; //RELEASE_REF

    if (numOfBytes == 0)
    {
        Disconnect(L"Recv 0");
        return;
    }

    if (_recvBuffer.OnWrite(numOfBytes) == false)
    {
        Disconnect(L"OnWrite Overflow");
        return;
    }

    int32 dataSize = _recvBuffer.DataSize();
    int32 processLen = OnRecv(_recvBuffer.ReadPos(), dataSize); // 컨텐츠 코드에서 오버라이딩
    if (processLen < 0 || dataSize < processLen || _recvBuffer.OnRead(processLen) == false)
    {
        Disconnect(L"OnRead Overflow");
        return;
    }
    
    // 커서 정리
    _recvBuffer.Clean();

    // 수신등록
    RegisterRecv();
}

void Session::ProcessSend(SendEvent* sendEvent, int32 numOfBytes)
{
    sendEvent->owner = nullptr; // RELEASE_REF
    xdelete(sendEvent);

    if (numOfBytes == 0)
    {
        Disconnect(L"Send 0");
        return;
    }

    // 컨텐츠 코드에서 오버라이딩
    OnSend(numOfBytes);
}

void Session::HandleError(int32 errorCode)
{
    switch (errorCode)
    {
    case WSAECONNRESET:
    case WSAECONNABORTED:
        Disconnect(L"HandleError");
        break;
    default:
        // TODO : Log
        cout << "Handle Error : " << errorCode << endl;
        break;
    }
}
