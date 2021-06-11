#include "pch.h"
#include "TimeProvider.h"


ClockTimeProvider::ClockTimeProvider()
{
    startTime = std::chrono::high_resolution_clock::now();
}

double ClockTimeProvider::GetTime()
{
    std::chrono::high_resolution_clock::time_point currentTime = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed_seconds = currentTime - startTime;
    return elapsed_seconds.count();
}

OscTimeProvider::OscTimeProvider(int port) : fTime(0.0f)
{
    socket.Bind(port);
}

void OscTimeProvider::handlePacket(const OSCPP::Server::Packet& packet)
{
    if (packet.isBundle()) {
        // Convert to bundle
        OSCPP::Server::Bundle bundle(packet);
        // Get packet stream
        OSCPP::Server::PacketStream packets(bundle.packets());

        // Iterate over all the packets and call handlePacket recursively.
        // Cuidado: Might lead to stack overflow!
        while (!packets.atEnd()) {
            handlePacket(packets.next());
        }
    }
    else {
        // Convert to message
        OSCPP::Server::Message msg(packet);

        // Get argument stream
        OSCPP::Server::ArgStream args(msg.args());

        // Directly compare message address to string with operator==.
        // For handling larger address spaces you could use e.g. a
        // dispatch table based on std::unordered_map.
        if (msg == "/Time") {
            fTime = args.float32();
        }
        else 
        {
            // Simply print unknown messages
            // std::cout << "Unknown message: " << msg.address() << std::endl;
        }
    }
}

double OscTimeProvider::GetTime()
{
    std::array<char, 1024> buffer;
    for(;;)
    { 
        int size = socket.RecvFrom(buffer.data(), (int)buffer.size());
        if(size < 0)
        {
            break;          // if no more packet needs to be processed 
        }
        handlePacket(OSCPP::Server::Packet(buffer.data(), size));
    }
    return fTime;
}


