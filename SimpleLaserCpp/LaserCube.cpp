#include "LaserCube.h"
#include "windows.h"


void LaserCube::ProcessFullInfo(const uint8_t* msg)
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

    m_modelname = std::string((char*)&msg[38]);
}

void LaserCube::ProcessMessage()
{
    for (;;)
    {
        if (m_pending_kill)
        {
            break;
        }
        std::array<uint8_t, 4096> cmd;
        int size = m_cmd_socket.RecvFrom((char*)cmd.data(), (int)cmd.size());
        if (size <= 0)
        {
            continue;
        }
        switch (cmd[0])
        {
        case  cmds::LASERCUBE_GET_FULL_INFO:
            if (size > 3)
            {
                ProcessFullInfo(cmd.data());
            }
            break;
        case cmds::LASERCUBE_ENABLE_BUFFER_SIZE_RESPONSE_ON_DATA:
            // nothing to do just acknowledging
            break;
        case cmds::LASERCUBE_CMD_GET_RINGBUFFER_EMPTY_SAMPLE_COUNT:
            m_buff_free = (int)cmd[2] + ((int)cmd[3] << 8);
            break;

        }
    }
}

void LaserCube::Initialize()
{
    for (;;)
    {
        sendCommand({ cmds::LASERCUBE_GET_FULL_INFO }, std::string("255.255.255.255"), 1);
        std::this_thread::sleep_for(std::chrono::microseconds(100));
        std::array<uint8_t, 4096> cmd;
        int size = m_cmd_socket.RecvFrom((char*)cmd.data(), (int)cmd.size());
        if (size >= 0 && cmd[0] == cmds::LASERCUBE_GET_FULL_INFO && size >= 3)
        {
            ProcessFullInfo(cmd.data());
            std::cout << "Found" << m_modelname << " Ip: " << m_ip_adress << "\n";
            break;
        }
    }
    sendCommand({ cmds::LASERCUBE_ENABLE_BUFFER_SIZE_RESPONSE_ON_DATA,  1 }, m_ip_adress);
    sendCommand({ cmds::LASERCUBE_CMD_SET_OUTPUT,  1 }, m_ip_adress);
}


void LaserCube::DrawSamples(const std::vector<LaserSample>& samples, int maxBufferSize)
{
    int sentSample = 0;
    while (sentSample != samples.size())
    {
        if (m_buff_free < (m_buff_size - maxBufferSize))
        {
            std::this_thread::sleep_for(std::chrono::microseconds(long long((1000000.0 * 100.0) / double(m_dac_rate))));
            m_buff_free += 100; // guesstimate
        }
        m_transfert_buff[0] = LASERCUBE_SAMPLE_DATA_ID;
        m_transfert_buff[1] = 0;
        m_transfert_buff[2] = m_message_num;
        m_transfert_buff[3] = m_frame_num;

        int sampleToSend = min((int)samples.size() - sentSample, constants::max_samples_per_udp_packet);
        memcpy(m_transfert_buff + 4, &samples[sentSample], sampleToSend * sizeof(LaserSample));

        //m_data_socket.SendTo(m_ip_adress, constants::data_port, (char *)m_transfert_buff, 4 + sizeof(LaserSample) * sampleToSend);
        m_cmd_socket.SendTo(m_ip_adress, constants::data_port, (char*)m_transfert_buff, 4 + sizeof(LaserSample) * sampleToSend);
        //sendCommand({ cmds::LASERCUBE_ENABLE_BUFFER_SIZE_RESPONSE_ON_DATA,  1 }, m_ip_adress);

        m_buff_free -= sampleToSend;
        sentSample += sampleToSend;
        m_message_num++;
    }

    if (samples.size())
    {
        m_frame_num++;
    }
}
