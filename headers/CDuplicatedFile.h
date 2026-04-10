//
// Created by vimax on 29.10.25.
//

#pragma once

#include <string>
#include <boost/filesystem.hpp>

class CDuplicatedFile
{
    public:
        CDuplicatedFile(const size_t size) : mSize(size){}
        CDuplicatedFile() = delete;

        void addPath(const boost::filesystem::path& path);

        [[nodiscard]] uint64_t getCount() const;
    
        [[nodiscard]] uint64_t getOccupiedSize() const;

        [[nodiscard]] uint64_t getPotentialFreeSize() const;
    
        void print() const;

        size_t mSize;
        std::list<boost::filesystem::path> mPathList;
};
