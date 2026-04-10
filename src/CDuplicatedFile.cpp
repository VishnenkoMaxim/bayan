//
// Created by vimax on 29.10.25.
//
#include <iostream>

#include "CDuplicatedFile.h"

void CDuplicatedFile::addPath(const boost::filesystem::path& path)
{
    mPathList.emplace_back(path);
}

uint64_t CDuplicatedFile::getCount() const
{
    return mPathList.size();   
}

uint64_t CDuplicatedFile::getOccupiedSize() const
{
    return mPathList.size() * mSize;
}

uint64_t CDuplicatedFile::getPotentialFreeSize() const
{
    if (mPathList.size() > 1)
    {
        return (mPathList.size()-1) * mSize;
    }
    return 0;
}

void CDuplicatedFile::print() const
{
    if (!mPathList.empty())
    {
        std::cout << "File size: " << mSize << std::endl;
        for (const auto& path : mPathList)
        {
            std::cout << "Path: " << path << std::endl;
        }
        std::cout << std::endl;
    }
}

