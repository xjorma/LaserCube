#include "UDPSocket.h"
#include <fstream>
#include <sstream>
#include <vector>
#include <array>
#include <iostream>
#include <chrono>
#include <thread>


int WSASession::m_count = 0;
WSAData  WSASession::data;


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

    uint8_t     *m_transfert_buff;


    void sendCommand(const std::vector<uint8_t> &msg, const std::string &addr, int repeat = 2)
    {
        for (int i = 0; i < repeat; i++)
        {
            m_cmd_socket.SendTo(std::string("255.255.255.255"), constants::cmd_port, (char*)msg.data(), (int)msg.size());
        }
    }

    void ProcessFullInfo(const uint8_t* msg)
    {
        m_fw_major = msg[3];
        m_fw_minor = msg[4];

        m_output_enabled = (bool)msg[5];

        m_dac_rate = (int)msg[10] + ((int)msg[11] << 8) + ((int)msg[12] << 16) + ((int)msg[13] << 24);

        m_max_dac_rate = (int)msg[14] + ((int)msg[15] << 8) + ((int)msg[16] << 16) + ((int)msg[17] << 24);

        m_buff_free = (int)msg[19] + ((int)msg[20] << 8);
        m_buff_size = (int)msg[21] + ((int)msg[22] << 8);

        m_battery_percent = (int)msg[23];

        m_temperature = (int)msg[24];

        m_connection_type = (int)msg[25];

        char sn[7];         // Serial number look strange
        sn[0] = msg[26];
        sn[1] = msg[27];
        sn[2] = msg[28];
        sn[3] = msg[29];
        sn[4] = msg[30];
        sn[5] = msg[31];
        sn[6] = '\0';
        m_serialnumber = std::string(sn);

        uint8_t ip[5];
        ip[0] = msg[32];
        ip[1] = msg[33];
        ip[2] = msg[34];
        ip[3] = msg[35];
        m_ip_adress = std::to_string(ip[0]) + "." + std::to_string(ip[1]) + "." + std::to_string(ip[2]) + "." + std::to_string(ip[3]);

        m_modelnum = (int)msg[37];

        m_modelname = std::string((char *)&msg[38]);
    }

    void ProcessMessage()
    {
        std::array<uint8_t, 4096> cmd;
        int size = m_cmd_socket.RecvFrom((char*)cmd.data(), (int)cmd.size());
        if (size <= 0)
        {
            return;
        }

        switch(cmd[0])
        {
        case  cmds::LASERCUBE_GET_FULL_INFO:
            ProcessFullInfo(cmd.data());
            break;
        case cmds::LASERCUBE_ENABLE_BUFFER_SIZE_RESPONSE_ON_DATA:
            // nothing to do just acknowledging
            break;
        case cmds::LASERCUBE_CMD_GET_RINGBUFFER_EMPTY_SAMPLE_COUNT:
            m_buff_free = (int)cmd[2] + ((int)cmd[3] << 8);
            break;

        }
    }

    void Initialize()
    {
        for(;;)
        {
            sendCommand({ cmds::LASERCUBE_GET_FULL_INFO }, std::string("255.255.255.255"), 1);
            std::this_thread::sleep_for(std::chrono::microseconds(100));
            std::array<uint8_t, 4096> cmd;
            int size = m_cmd_socket.RecvFrom((char *)cmd.data(), (int)cmd.size());
            if (size >= 0 && cmd[0] == cmds::LASERCUBE_GET_FULL_INFO && size >= 3)
            {
                ProcessFullInfo(cmd.data());
                std::cout << "Found" << m_modelname << "Ip: " << m_ip_adress << "Sn:" << m_serialnumber << "\n";
                break;
            }
        }
        sendCommand({ cmds::LASERCUBE_ENABLE_BUFFER_SIZE_RESPONSE_ON_DATA,  1 }, m_ip_adress);
        sendCommand({ cmds::LASERCUBE_CMD_SET_OUTPUT,  1 }, m_ip_adress);
    }


public:
    LaserCube() : m_frame_num(0), m_message_num(0), m_transfert_buff(new uint8_t[4 + sizeof(LaserSample) * constants::max_samples_per_udp_packet]), m_cmd_socket(1), m_data_socket(0)
    {
        m_cmd_socket.SetBroadCast();
        m_cmd_socket.Bind(constants::cmd_port);
        m_data_socket.Bind(constants::data_port);
        Initialize();

    }

    ~LaserCube()
    {
        delete m_transfert_buff;
        sendCommand({ cmds::LASERCUBE_ENABLE_BUFFER_SIZE_RESPONSE_ON_DATA,  0}, m_ip_adress);
        sendCommand({ cmds::LASERCUBE_CMD_SET_OUTPUT,  0 }, m_ip_adress);
    }

    void DrawSample(const std::vector<LaserSample> &samples, int maxBufferSize = 1000)
    {
        ProcessMessage();
        if (m_buff_free < (m_buff_size - maxBufferSize))
        {
            std::this_thread::sleep_for(std::chrono::microseconds((1000000 * 100) / m_dac_rate));
            m_buff_free += 10; // guesstimate
        }
        
        int sentSample = 0;
        while (sentSample != samples.size())
        {
            m_transfert_buff[0] = LASERCUBE_SAMPLE_DATA_ID;
            m_transfert_buff[1] = 0;
            m_transfert_buff[2] = m_message_num;
            m_transfert_buff[3] = m_frame_num;

            int sampleToSend = min((int)samples.size() - sentSample, constants::max_samples_per_udp_packet);
            memcpy(m_transfert_buff + 4, &samples[sentSample], sampleToSend * sizeof(LaserSample));

            m_data_socket.SendTo(m_ip_adress, constants::data_port, (char *)m_transfert_buff, 4 + sizeof(LaserSample) * sampleToSend);
            m_buff_free -= sampleToSend;
            sentSample += sampleToSend;
            m_message_num++;
        }

        if (samples.size())
        {
            m_frame_num++;
        }
    }
};


int main()
{
    LaserCube laserCube;

    for(;;)
    {
        std::vector<LaserSample> samples;
        samples.reserve(256);
        float time = 0.0f;
        const float pi = 3.14159265359f;
        int pointInShape = 50;
        for (int i = 0; i < pointInShape; i++)
        {
            float p = float(i) / float(pointInShape);
            float intensity = 16.0f;
            LaserSample curSample;
            curSample.x = int(((sin(p * pi * 2))  * 0.7 / 2. + 0.5) * 0xfff);
            curSample.y = int(((cos(p * pi * 2))  * 0.7 / 2. + 0.5) * 0xfff);
            //curSample.r = int(pow((sin((p + (time * 1)) * (pi * 4)) / 2. + 0.5), 1) * intensity);
            //curSample.g = int(pow((sin((p + (time * 2)) * (pi * 4)) / 2. + 0.5), 1) * intensity);
            //curSample.b = int(pow((sin((p + (time * 3)) * (pi * 4)) / 2. + 0.5), 1) * intensity);
            curSample.r = 128;
            curSample.g = 0;
            curSample.b = 0;
            samples.push_back(curSample);
        }
        laserCube.DrawSample(samples);
    }


}

