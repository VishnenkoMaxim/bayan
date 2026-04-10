//
// Created by vimax on 31.10.25.
//

#include <iostream>
#include <boost/asio.hpp>

#include "CDuplicatedFile.h"
#include "gui_interface/CGUIInterface.h"
#include "bayan.h"

using namespace boost::asio::ip;

bool CBoostSocketGUI::connect()
{
    try
    {
        tcp::resolver resolver(mIOContext);
        const tcp::resolver::results_type endpoints = resolver.resolve("localhost", std::to_string(GUI_PORT));

        boost::asio::connect(mSocket, endpoints);

        std::cout << "Successfully connected to " << mSocket.remote_endpoint() << std::endl;
        return true;
    }
    catch (const std::exception& e)
    {
        std::cout << e.what() << std::endl;
        return false;
    }
}

bool CBoostSocketGUI::sendData(const char* data, const std::size_t buf_size)
{
    try
    {
        if (!mSocket.is_open())
        {
            if (!connect())
            {
                return false;
            }
        }

        boost::asio::write(mSocket, boost::asio::buffer(data, buf_size));

        std::cout << "Sent " << buf_size << " to client " << std::endl;
        return true;
    }
    catch (const boost::system::system_error& e)
    {
       std::cout << "Error writing to socket: " << e.code().message() << ", (" << e.code().value() << ")" << std::endl;
       return false;
    }
}

std::unique_ptr<char []> CBoostSocketGUI::readData(const std::size_t buf_size) {
    try
    {
        if (!mSocket.is_open())
        {
            if (!connect())
            {
                return {};
            }
        }
        std::unique_ptr<char []> data(new char[buf_size]);

        boost::asio::read(mSocket, boost::asio::buffer(data.get(), buf_size));
        std::cout << "Read " << buf_size << " from client " << std::endl;

        mIOContext.run();
        return data;
    }
    catch (const boost::system::system_error& e)
    {
        std::cout << "Error reading from socket: " << e.code().message() << ", (" << e.code().value() << ")" << std::endl;
        return {};
    }
}

CBoostSocketGUI::~CBoostSocketGUI()
{
    try
    {
        mSocket.close();
    }
    catch (const boost::system::system_error& e)
    {
        std::cout << "Couldn't close socket: " << e.what() << std::endl;
    }
}