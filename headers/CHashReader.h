//
// Created by vimax on 26.10.25.
//

#pragma once
#include <functional>
#include <iostream>
#include <mutex>
#include <queue>
#include <thread>
#include <boost/filesystem/path.hpp>

struct ReadTaskData {
    boost::filesystem::path path;
    uint32_t& offset;
    uint32_t& hash;
    bool& err;
};

#define DEFAULT_THREADS_NUM 4

class CHashReader {
    public:
    CHashReader(const uint32_t block_size, uint32_t (*hash_func)(const char*, uint32_t), const uint8_t threads_num = 0)
        : mBlockSize(block_size), mHashFunc(hash_func), mThreadsNum(threads_num)
    {
        if (mThreadsNum == 0)
        {
            #ifdef __linux__
                mThreadsNum = std::thread::hardware_concurrency()/2;
            #endif
                    
            #ifdef _WIN32
                mThreadsNum = DEFAULT_THREADS_NUM;
            #endif
                    
            #ifdef __APPLE__
                mThreadsNum = DEFAULT_THREADS_NUM;
            #endif
        }
    }

    void addTask(const ReadTaskData& task);

    void wait();

    void start();

    void init();
    
private:
    std::mutex mMutex;
    std::queue<ReadTaskData> mQueue;

    uint32_t mBlockSize = 1024;
    std::function<uint32_t(const char*, uint32_t)> mHashFunc;
    uint8_t mThreadsNum = 4;
    std::vector<std::thread> mThreads;

    void reader();

    uint32_t calcBlockHash(std::ifstream &_stream, uint32_t& offset);
};
