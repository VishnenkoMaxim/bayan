//
// Created by vimax on 26.10.25.
//

#include "CHashReader.h"

#include <fstream>
#include <iostream>
#include <boost/filesystem/exception.hpp>

void CHashReader::init()
{
    std::unique_lock<std::mutex> lock(mMutex);
    while (!mQueue.empty()) mQueue.pop();
}

void CHashReader::addTask(const ReadTaskData& task) {
    //std::unique_lock<std::mutex> lock(mMutex);
    mQueue.push(task);
}

void CHashReader::wait() {
    if (mThreads.empty()) {
        start();
    }

    for (auto& thread : mThreads) {
        thread.join();
    }
    mThreads.clear();
}

void CHashReader::start() {
    while (mThreads.size() < mThreadsNum) {
        mThreads.emplace_back(std::bind(&CHashReader::reader, this));
    }
}

uint32_t CHashReader::calcBlockHash(std::ifstream &_stream, uint32_t& offset) {
    const auto buf = std::make_unique<char[]>(mBlockSize);
    _stream.read(buf.get(), mBlockSize);

    if (_stream) {
        offset += mBlockSize;
        return mHashFunc(buf.get(), mBlockSize);
    }

    offset += _stream.gcount();
    return mHashFunc(buf.get(), mBlockSize);
}

void CHashReader::reader() {
    while (!mQueue.empty()) {
        std::unique_lock<std::mutex> lock(mMutex);
        if (mQueue.empty()) {
            return;
        }

        auto task = mQueue.front();
        mQueue.pop();
        lock.unlock();

        try
        {
            std::ifstream file;
            file.open(task.path, std::ifstream::binary);
            if (file.good()){
                file.seekg(task.offset);
                task.hash = calcBlockHash(file, task.offset);
                file.close();
                task.err = false;
            } else {
                std::cout << "open error: " <<  task.path.string() << std::endl;
                task.err = true;
            }
        }
        catch (const boost::filesystem::filesystem_error& e)
        {
            std::cout << e.what() << std::endl;
            task.err = true;
        }
        catch (std::exception& e)
        {
            std::cout << e.what() << std::endl;
            task.err = true;
        }
    }
}
