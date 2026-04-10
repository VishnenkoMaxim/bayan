//
// Created by vimax on 31.10.25.
//

#include <iostream>
#include <boost/asio.hpp>

#include "CDuplicatedFile.h"
#include "gui_interface/CGUIInterface.h"
#include "bayan.h"

using namespace boost::asio::ip;

void CBoostSocketGUI::handleWriteConnect(const boost::system::error_code& error, const tcp::endpoint& endpoint,
                                    const char* data, std::size_t buf_size)
{
    if (!error) {
        std::cout << "Successfully connected to " << endpoint << std::endl;

        boost::asio::async_write(mSocket, boost::asio::buffer(data, buf_size), [endpoint](const boost::system::error_code &ec, const std::size_t bytes)
        {
            if (ec)
            {
                std::cout << "Error writing to socket: " << ec.message() << std::endl;
                return;
            }
            std::cout << "Sent " << bytes << " to client " << endpoint << std::endl;
        });
    } else {
        std::cerr << "Connection error: " << error.message() << std::endl;
    }
}

bool CBoostSocketGUI::sendData(const char* data, const std::size_t buf_size)
{
    try
    {
        tcp::resolver resolver(mIOContext);
        const tcp::resolver::results_type endpoints = resolver.resolve("localhost", std::to_string(GUI_PORT));
        
        boost::asio::async_connect(mSocket, endpoints,
           [&](const boost::system::error_code& error, const tcp::endpoint& connected_endpoint) {
               handleWriteConnect(error, connected_endpoint, data, buf_size);
           });
        
        mIOContext.run();
        
        return true;
    }
    catch (const std::exception& e)
    {
       std::cout << "exception " << e.what() << std::endl;
        return false;
    }
}

void CBoostSocketGUI::handleReadConnect(const boost::system::error_code& error, const tcp::endpoint& endpoint,
                        char* data, const std::size_t buf_size)
{
    if (!error) {
        std::cout << "Successfully connected to " << endpoint << std::endl;

        boost::asio::async_read(mSocket, boost::asio::buffer(data, buf_size), [endpoint](const boost::system::error_code &ec, const std::size_t bytes)
        {
            if (ec)
            {
                std::cout << "Error reading from socket: " << ec.message() << std::endl;
                return;
            }
            std::cout << "Read " << bytes << " from client " << endpoint << std::endl;
        });
    } else {
        std::cerr << "Connection error: " << error.message() << std::endl;
    }
}

std::unique_ptr<char []> CBoostSocketGUI::readData(const std::size_t buf_size) {
    std::unique_ptr<char []> data(new char[buf_size]);

    try
    {
        tcp::resolver resolver(mIOContext);
        const tcp::resolver::results_type endpoints = resolver.resolve("localhost", std::to_string(GUI_PORT));

        boost::asio::async_connect(mSocket, endpoints,
           [&](const boost::system::error_code& error, const tcp::endpoint& connected_endpoint) {
               handleReadConnect(error, connected_endpoint, data.get(), buf_size);
           });

        mIOContext.run();

        return data;
    }
    catch (const std::exception& e)
    {
        std::cout << "exception " << e.what() << std::endl;
        return data;
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