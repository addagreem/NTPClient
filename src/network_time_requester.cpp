#include <iostream>
#include <system_error>
#include <list>
#include <iomanip>

#include "blocking_client.hpp"
#include "ntp_packet.hpp"
#include "network_time_requester.hpp"

NetworkTimeRequester::NetworkTimeRequester(bool logging)
    : mLogging(logging)
{}

std::time_t NetworkTimeRequester::getTimestamp() const
{
    constexpr std::time_t NTP_TIMESTAMP_DELTA = 2208988800U; // Unix time starts from 01-01-1970 == 2208988800U
    constexpr ushort TIMEOUT = 10;
    auto pool = getServerPool();

    std::time_t timeRecv = 0;
    for (auto& host : pool)
    {
        try
        {
            NtpPacket ntpPacket = {};
            ntpPacket.packet.li_vn_mode =  0x1B; // (version = 3, mode = 3)
            UdpBlockingClient ntpClient(host, "ntp");

            asio::error_code ec;
            ntpClient.send(asio::buffer(ntpPacket.buf), TIMEOUT, ec);
            if(ec)
                throw std::runtime_error("Send error: " + ec.message() + (ec == asio::error::operation_aborted ? " due to timeout" : ""));

            ntpClient.receive(asio::buffer(ntpPacket.buf), TIMEOUT, ec);
            if(ec)
                throw std::runtime_error("Receive error: " + ec.message() + (ec == asio::error::operation_aborted ? " due to timeout" : ""));

            ntpPacket.packet.txTm_s = ntohl(ntpPacket.packet.txTm_s);
            if (ntpPacket.packet.txTm_s == 0)
                throw std::runtime_error("Received wrong timestamp value (0) from " + std::string(host));

            timeRecv = ntpPacket.packet.txTm_s - NTP_TIMESTAMP_DELTA;

            if (mLogging)
                std::cout << std::put_time(std::gmtime(&timeRecv), "%FT%T.000Z") << " - got from " << host << std::endl;

            break;
        }
        catch (const std::exception& e)
        {
            if (mLogging)
                std::cerr << "NetworkTimeRequester::" << __FUNCTION__ << ": " << e.what() << std::endl;
        }
    }

    return timeRecv;
}

std::list<std::string> NetworkTimeRequester::getServerPool() const
{
    return std::list<std::string>  {
        "0.pool.ntp.org",
        "1.pool.ntp.org",
        "2.pool.ntp.org"
        "3.pool.ntp.org"
    };
}


int main()
{
    NetworkTimeRequester(true).getTimestamp();
    return 0;
}
