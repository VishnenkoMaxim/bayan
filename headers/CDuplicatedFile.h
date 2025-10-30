//
// Created by vimax on 29.10.25.
//

#pragma once

#include <string>
#include <boost/filesystem.hpp>

class CDuplicatedFile
{
    public:
        CDuplicatedFile(const size_t size, const std::string &file_name) : mSize(size), mName(file_name){}
        CDuplicatedFile() = delete;
    
        size_t mSize;
        std::string mName;

        void addPath(const boost::filesystem::path& path);

        uint64_t getCount() const;
    
        uint64_t getOccupiedSize() const;

        uint64_t getPotentialFreeSize() const;
    
        void print() const;
    
    private:
        std::list<boost::filesystem::path> mPathList;
};
