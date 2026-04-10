//
// Created by vimax on 31.10.25.
//

#pragma once

class IGUIInterface
{
public:
    virtual bool sendData(const char* data, std::size_t buf_size) = 0;

    virtual std::unique_ptr<char []> readData(std::size_t buf_size) = 0;

    virtual ~IGUIInterface() = default;
};

class CBoostSocketGUI : public IGUIInterface
{
public:
    explicit CBoostSocketGUI() : mSocket(mIOContext) {}

    bool sendData(const char* data, std::size_t buf_size) override;

    std::unique_ptr<char []> readData(std::size_t buf_size) override;

    ~CBoostSocketGUI() override;

private:
    boost::asio::io_context mIOContext;
    boost::asio::ip::tcp::socket mSocket;

    bool connect();
};
