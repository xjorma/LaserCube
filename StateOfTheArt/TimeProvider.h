#pragma once

class TimeProvider
{
public:
	TimeProvider()
	{}
	virtual double GetTime() = 0;
};

class ClockTimeProvider : public TimeProvider
{
private:
	std::chrono::high_resolution_clock::time_point startTime;
public:
	ClockTimeProvider();
	double GetTime() override;
};

class WSASession
{
public:
    WSASession()
    {
        int ret = WSAStartup(MAKEWORD(2, 2), &data);
        if (ret != 0)
            throw std::system_error(WSAGetLastError(), std::system_category(), "WSAStartup Failed");
    }
    ~WSASession()
    {
        WSACleanup();
    }
private:
    WSAData data;
};

class UDPSocket
{
public:
    UDPSocket()
    {
        sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
        if (sock == INVALID_SOCKET)
            throw std::system_error(WSAGetLastError(), std::system_category(), "Error opening socket");
        u_long iMode = 1;
        int iResult = ioctlsocket(sock, FIONBIO, &iMode);
        if (iResult != NO_ERROR)
            throw std::system_error(WSAGetLastError(), std::system_category(), "Cannot set socket to non-blocking");
    }
    ~UDPSocket()
    {
        closesocket(sock);
    }

    void SendTo(const std::string& address, unsigned short port, const char* buffer, int len, int flags = 0)
    {
        sockaddr_in add;
        add.sin_family = AF_INET;
        add.sin_addr.s_addr = inet_addr(address.c_str());
        add.sin_port = htons(port);
        int ret = sendto(sock, buffer, len, flags, reinterpret_cast<SOCKADDR*>(&add), sizeof(add));
        if (ret < 0)
            throw std::system_error(WSAGetLastError(), std::system_category(), "sendto failed");
    }
    void SendTo(sockaddr_in& address, const char* buffer, int len, int flags = 0)
    {
        int ret = sendto(sock, buffer, len, flags, reinterpret_cast<SOCKADDR*>(&address), sizeof(address));
        if (ret < 0)
            throw std::system_error(WSAGetLastError(), std::system_category(), "sendto failed");
    }
    int RecvFrom(char* buffer, int len, int flags = 0)
    {
        sockaddr_in from;
        int size = sizeof(from);
        int ret = recvfrom(sock, buffer, len, flags, reinterpret_cast<SOCKADDR*>(&from), &size);
        if (ret < -1)
        {
            throw std::system_error(WSAGetLastError(), std::system_category(), "recvfrom failed");
        }
        else if (ret == -1)
        {
            return ret;
        }
        else
        {
            // make the buffer zero terminated
            buffer[ret] = 0;
        }
        return ret;
    }
    void Bind(unsigned short port)
    {
        sockaddr_in add;
        add.sin_family = AF_INET;
        add.sin_addr.s_addr = htonl(INADDR_ANY);
        add.sin_port = htons(port);

        int ret = bind(sock, reinterpret_cast<SOCKADDR*>(&add), sizeof(add));
        if (ret < 0)
            throw std::system_error(WSAGetLastError(), std::system_category(), "Bind failed");
    }

private:
    SOCKET sock;
};

class OscTimeProvider : public TimeProvider
{
    WSASession session;
    UDPSocket socket;

private:
	float fTime;
public:
	OscTimeProvider(int port);
	double GetTime() override;
    void handlePacket(const OSCPP::Server::Packet& packet);
};

