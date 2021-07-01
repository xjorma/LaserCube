#pragma once

#include "UDPSocket.h"
#include <fstream>
#include <sstream>
#include <vector>
#include <array>
#include <iostream>
#include <chrono>
#include <thread>

struct LaserSample
{
    uint16_t x;
    uint16_t y;
    uint16_t r;
    uint16_t g;
    uint16_t b;
};

namespace cmds {
    const uint8_t LASERCUBE_GET_ALIVE = 0x27;
    const uint8_t LASERCUBE_GET_FULL_INFO = 0x77;
    const uint8_t LASERCUBE_ENABLE_BUFFER_SIZE_RESPONSE_ON_DATA = 0x78;
    const uint8_t LASERCUBE_CMD_SET_OUTPUT = 0x80;
    const uint8_t LASERCUBE_CMD_SET_ILDA_RATE = 0x82;
    const uint8_t LASERCUBE_CMD_CLEAR_RINGBUFFER = 0x8d;
    const uint8_t LASERCUBE_CMD_GET_RINGBUFFER_EMPTY_SAMPLE_COUNT = 0x8a;
    const uint8_t LASERCUBE_CMD_SET_NV_MODEL_INFO = 0x97;
    const uint8_t LASERCUBE_SECURITY_CMD_REQUEST = 0xb0;
    const uint8_t LASERCUBE_SECURITY_CMD_RESPONSE = 0xb1;
}

namespace constants {

    const uint8_t min_protocol_ver = 0;                 // this defines the protocol version of the full info response packet from network cube
    const int alive_port = 45456;                       // this port accepts alive requests, and responds with alive messages only
    const int cmd_port = 45457;                         // this port accepts all the network cube commands
    const int data_port = 45458;                        // this port only accepts sample data to render
    const int comms_timeout_period_ms = 4000;           // how long to wait for a full cube info response pkt before we flag as disconnected
    const int inactive_info_request_period_ms = 250;    // how often to poll for the full cube info status while laser is inactive
    const int active_info_request_period_ms = 2500;    // how often to poll for the full cube info status while laser is active
    const int thread_sample_buffer_size = 8000;         // how big the buffer between threadworker class and our udp class
    const int max_udp_packets = 20;                     // maximum number of UDP packets to send in one go (ESP32 has an internal 20 UDP limit)
    const int max_samples_per_udp_packet = 140;          // max bytes we should ever send in one UDP packet (80 uncompressed samples + 4 byte header)
    const int command_repeat_count = 2;                 // how many times to send out a command over UDP
    const int security_timeout_ms = 1000;               // how long to wait for a security response after a request was sent
}

// UDP packets on the data port have this id byte as the first byte of the packet for standard uncompressed sample data
#define LASERCUBE_SAMPLE_DATA_ID						0xA9


class LaserCube
{
    UDPSocket   m_cmd_socket;
    UDPSocket   m_data_socket;
    uint8_t     m_fw_major;
    uint8_t     m_fw_minor;
    bool        m_output_enabled;
    int         m_dac_rate;
    int         m_max_dac_rate;
    int         m_buff_free;
    int         m_buff_size;
    int         m_battery_percent;
    int         m_temperature;
    int         m_connection_type;
    std::string m_serialnumber;
    std::string m_ip_adress;
    int         m_modelnum;
    std::string m_modelname;

    char        m_frame_num;
    char        m_message_num;

    uint8_t* m_transfert_buff;

    bool        m_pending_kill;
    std::thread* m_process_message_thread;


    void sendCommand(const std::vector<uint8_t>& msg, const std::string& addr, int repeat = 2)
    {
        for (int i = 0; i < repeat; i++)
        {
            m_cmd_socket.SendTo(addr, constants::cmd_port, (char*)msg.data(), (int)msg.size());
        }
    }

    void ProcessFullInfo(const uint8_t* msg);

    void ProcessMessage();

    void Initialize();


public:
    LaserCube() : m_frame_num(0), m_message_num(0), m_transfert_buff(new uint8_t[4 + sizeof(LaserSample) * constants::max_samples_per_udp_packet]), m_cmd_socket(0), m_data_socket(0), m_pending_kill(false)
    {
        m_cmd_socket.SetBroadCast();
        m_cmd_socket.Bind(constants::cmd_port);
        m_data_socket.Bind(constants::data_port);
        Initialize();
        m_process_message_thread = new std::thread(&LaserCube::ProcessMessage, this);
    }

    ~LaserCube()
    {
        delete m_transfert_buff;
        m_pending_kill = true;
        sendCommand({ cmds::LASERCUBE_ENABLE_BUFFER_SIZE_RESPONSE_ON_DATA,  0 }, m_ip_adress);
        sendCommand({ cmds::LASERCUBE_CMD_SET_OUTPUT,  0 }, m_ip_adress);
        m_process_message_thread->join();
        delete m_process_message_thread;
        delete[] m_transfert_buff;
    }

    void DrawSamples(const std::vector<LaserSample>& samples, int maxBufferSize = 1000);
};
