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
    UdpListeningReceiveSocket s( IpEndpointName(IpEndpointName::ANY_ADDRESS, port), &listener);
}

double OscTimeProvider::GetTime()
{
    return fTime;
}

void OscReceive::ProcessMessage(const osc::ReceivedMessage& m, const IpEndpointName& remoteEndpoint)
{
    osc::ReceivedMessageArgumentStream args = m.ArgumentStream();
    osc::ReceivedMessage::const_iterator arg = m.ArgumentsBegin();
    if (strcmp(m.AddressPattern(), "/time") == 0)
    {
        float fTime;
        args >> fTime >> osc::EndMessage;
    }
}
