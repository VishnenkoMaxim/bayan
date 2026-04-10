//
// Created by vimax on 30.10.25.
//

#pragma once
#include <cstdint>

namespace server_interface
{
    enum class Function : uint8_t
    {
        get_progress,
        get_scan_result
    };
    
    struct Header
    {
        Function func;
        uint64_t len;
    };
    
}

