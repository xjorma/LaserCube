#pragma once

#include "osc/OscReceivedElements.h"
#include "ip/UdpSocket.h"
#include "osc/OscPacketListener.h"

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

class OscReceive : public osc::OscPacketListener
{
protected:
	void ProcessMessage(const osc::ReceivedMessage& m, const IpEndpointName& remoteEndpoint);
};

class OscTimeProvider : public TimeProvider
{
private:
	OscReceive listener;
	float fTime;
public:
	OscTimeProvider(int port);
	double GetTime() override;
};