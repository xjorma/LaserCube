#pragma once
#include "UDPSOcket.h"

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


class OscTimeProvider : public TimeProvider
{
    UDPSocket socket;

private:
	float fTime;
public:
	OscTimeProvider(int port);
	double GetTime() override;
    void handlePacket(const OSCPP::Server::Packet& packet);
};

