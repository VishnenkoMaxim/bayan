//
// Created by vimax on 31.10.25.
//

#include <iostream>
#include <boost/asio.hpp>

#include "CDuplicatedFile.h"
#include "CGUIInterface.h"
#include "bayan.h"

using namespace boost::asio::ip;

void handleConnect(const boost::system::error_code& error, tcp::endpoint endpoint)
{
    if (!error) {
        std::cout << "Successfully connected to " << endpoint << std::endl;
    } else {
        std::cerr << "Connection error: " << error.message() << std::endl;
    }
}

bool CBoostSocketGUI::sendResults(const std::vector<CDuplicatedFile>& results)
{
    try
    {
        tcp::resolver resolver(mIOContext);
        const tcp::resolver::results_type endpoints = resolver.resolve("localhost", std::to_string(GUI_PORT));
        
        boost::asio::async_connect(mSocket, endpoints,
           [&](const boost::system::error_code& error, const tcp::endpoint& connected_endpoint) {
               handleConnect(error, connected_endpoint);
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