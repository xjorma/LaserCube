#pragma once
#include <string>
#include <iostream>
#include <WinSock2.h>
#include <Ws2tcpip.h>

class WSASession
{
public:
    WSASession()
    {
        if (!m_count)
        {
            int ret = WSAStartup(MAKEWORD(2, 2), &data);
            if (ret != 0)
                throw std::system_error(WSAGetLastError(), std::system_category(), "WSAStartup Failed");
        }
        m_count++;
    }
    ~WSASession()
    {
        if (!m_count)
        {
            WSACleanup();
        }
    }
private:
    static int m_count;
    static WSAData data;
};

class UDPSocket
{
public:
    UDPSocket(u_long iMode = 1)
    {
        m_sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
        if (m_sock == INVALID_SOCKET)
            throw std::system_error(WSAGetLastError(), std::system_category(), "Error opening socket");
        int iResult = ioctlsocket(m_sock, FIONBIO, &iMode);
        if (iResult != NO_ERROR)
            throw std::system_error(WSAGetLastError(), std::system_category(), "Cannot set socket to non-blocking");
    }
    ~UDPSocket()
    {
        closesocket(m_sock);
    }

    void SendTo(const std::string& address, unsigned short port, const char* buffer, int len, int flags = 0)
    {
        sockaddr_in add;
        add.sin_family = AF_INET;
        InetPton(AF_INET, std::wstring(address.begin(), address.end()).c_str(), &add.sin_addr.s_addr);
        add.sin_port = htons(port);
        int ret = sendto(m_sock, buffer, len, flags, reinterpret_cast<SOCKADDR*>(&add), sizeof(add));
        if (ret < 0)
            throw std::system_error(WSAGetLastError(), std::system_category(), "sendto failed");
    }
    void SendTo(sockaddr_in& address, const char* buffer, int len, int flags = 0)
    {
        int ret = sendto(m_sock, buffer, len, flags, reinterpret_cast<SOCKADDR*>(&address), sizeof(address));
        if (ret < 0)
            throw std::system_error(WSAGetLastError(), std::system_category(), "sendto failed");
    }
    int RecvFrom(char* buffer, int len, int flags = 0)
    {
        sockaddr_in from;
        int size = sizeof(from);
        int ret = recvfrom(m_sock, buffer, len, flags, reinterpret_cast<SOCKADDR*>(&from), &size);
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

        int ret = bind(m_sock, reinterpret_cast<SOCKADDR*>(&add), sizeof(add));
        if (ret < 0)
            throw std::system_error(WSAGetLastError(), std::system_category(), "Bind failed");
    }

    void SetBroadCast()
    {
        char broadcast = '1';
        int ret = setsockopt(m_sock, SOL_SOCKET, SO_BROADCAST, &broadcast, sizeof(broadcast));
        if (ret < 0)
            throw std::system_error(WSAGetLastError(), std::system_category(), "Broadcast failed");
    }

private:
    WSASession m_WSASession;
    SOCKET m_sock;
};