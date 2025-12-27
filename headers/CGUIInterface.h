//
// Created by vimax on 31.10.25.
//

#pragma once

class IGUIInterface
{
public:

    virtual bool sendResults(const std::vector<CDuplicatedFile>& results) = 0;
    virtual bool sendProgress() = 0;

    virtual ~IGUIInterface() = default;
};

class CBoostSocketGUI : public IGUIInterface
{
public:
    explicit CBoostSocketGUI(boost::asio::io_context& context) : mIOContext(context), mSocket(mIOContext) {}

    bool sendResults(const std::vector<CDuplicatedFile>& results) override;

private:
    boost::asio::io_context& mIOContext;
    boost::asio::ip::tcp::socket mSocket;

    void handleConnect(const boost::system::error_code& error, boost::asio::ip::tcp::endpoint endpoint);
};
