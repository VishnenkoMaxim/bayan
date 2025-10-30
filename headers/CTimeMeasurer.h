//
// Created by vimax on 30.10.25.
//

#pragma once

#include <chrono>
#include <iostream>

class CTimeMeasurer {
public:
    void start() {
        start_time = std::chrono::system_clock::now();
    }

    void stop() {
        stop_time = std::chrono::system_clock::now();
    }

    void print() const
    {
        const auto [sec, m_sec] = getDurationSec();
        std::cout << "Total elapsed time: " << sec << "." << m_sec << " sec" << std::endl;
    }

    [[nodiscard]] std::pair<unsigned long, unsigned long> getElapsedTime() const
    {
        return getDurationSec();
    }
    
private:
    std::chrono::system_clock::time_point start_time;
    std::chrono::system_clock::time_point stop_time;

    [[nodiscard]] std::pair<unsigned long, unsigned long> getDurationSec() const
    {
        const auto seconds = std::chrono::duration_cast<std::chrono::seconds>(stop_time-start_time).count();
        const auto m_sec = std::chrono::duration_cast<std::chrono::milliseconds>(stop_time-start_time).count() - seconds*100;
        
        return std::make_pair(seconds, m_sec);
    }
};
