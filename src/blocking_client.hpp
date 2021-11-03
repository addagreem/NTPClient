#pragma once

#include <asio.hpp>

template<typename T>
class BlockingClient
{
public:
    BlockingClient(const std::string& host, const std::string& port)
        : mSocket(mIoContext)
        , mDeadline(mIoContext, std::chrono::seconds::max())
    {
        typename T::resolver resolver(mIoContext);
        asio::connect(mSocket, resolver.resolve(host, port));
        checkDeadline();
    }

    std::size_t receive(const asio::mutable_buffer& buffer, ushort timeout, asio::error_code& ec)
    {
        std::size_t length = 0;
        ec = asio::error::would_block;

        mDeadline.expires_after(std::chrono::seconds(timeout));
        mSocket.async_receive(asio::buffer(buffer), std::bind(&BlockingClient::completion_cb, this, std::placeholders::_1, std::placeholders::_2, &ec, &length));

        // Block until the asynchronous operation has completed.
        while (ec == asio::error::would_block)
            mIoContext.run_one();

        return length;
    }

    std::size_t send(const asio::mutable_buffer& buffer, ushort timeout, asio::error_code& ec)
    {
        std::size_t length = 0;
        ec = asio::error::would_block;

        mDeadline.expires_after(std::chrono::seconds(timeout));
        mSocket.async_send(asio::buffer(buffer), std::bind(&BlockingClient::completion_cb, this, std::placeholders::_1, std::placeholders::_2, &ec, &length));

        // Block until the asynchronous operation has completed.
        while (ec == asio::error::would_block)
            mIoContext.run_one();

        return length;
    }

private:
    void checkDeadline()
    {
        if (mDeadline.expiry() <= std::chrono::steady_clock::now())
        {
            mSocket.cancel();
            mDeadline.expires_after(std::chrono::seconds::max());
        }

        mDeadline.async_wait(std::bind(&BlockingClient::checkDeadline, this));
    }

    void completion_cb(const asio::error_code& ec, std::size_t length, asio::error_code* out_ec, std::size_t* out_length)
    {
        *out_ec = ec;
        *out_length = length;
    }

private:
    asio::io_context mIoContext;
    typename T::socket mSocket;
    asio::steady_timer mDeadline;
};


using UdpBlockingClient = BlockingClient<asio::ip::udp>;
using TcpBlockingClient = BlockingClient<asio::ip::tcp>;
