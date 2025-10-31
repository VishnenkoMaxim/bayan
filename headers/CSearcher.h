//
// Created by vimax on 30.10.25.
//

#pragma once

#include "bayan.h"
#include "CTimeMeasurer.h"

class CSearcher
{
    public:
        friend class CSearcherBuilder;

        CSearcher()
        {
            mAllFiles.reserve(1024);
            mScanFolders.reserve(1024);
            mDuplicatedFiles.reserve(1024);
        }
    
        void traverse();

        void search();
    
        void printSettings() const;
    
    private:

        std::vector<FileData> traverseDir(const fs::path& dir) const;
        static bool checkFilters(const fs::path &path, const std::vector<std::string> &_mask_vector);
        std::vector<FileData> FindDuplicates(const std::unordered_multimap<uint32_t, FileData> &src, std::vector<CDuplicatedFile>& duplicated_files) const;
    
        std::vector<std::string> mExcludedFolders;
        std::vector<std::string> mFilters;
        std::vector<std::string> mScanFolders;
        std::vector<FileData> mAllFiles;
    
        uint32_t mMinFileSize = 1;
        uint32_t mBlockSize = 16384;
        bool isRecursive = true;
        std::string mHashAlg;
    
        std::vector<CDuplicatedFile> mDuplicatedFiles;
        std::unordered_multimap<uint32_t, FileData> data;
        CTimeMeasurer mTimeMeasurer;
};

class CSearcherBuilder
{
public:
    CSearcherBuilder& withFilters(const std::vector<std::string>& filters);
    CSearcherBuilder& withMinFileSize(uint32_t min_file_size);
    CSearcherBuilder& withBlockSize(uint32_t block_size);
    CSearcherBuilder& withRecursive(bool is_recursive);
    CSearcherBuilder& withHashAlg(const std::string& hash_alg);
    CSearcherBuilder& withExcludedFolders(const std::vector<std::string>& excluded_folders);
    CSearcherBuilder& withFoldersToScan(const std::vector<std::string>& scan_folders);
    
    CSearcher& build();

private:
    CSearcher mSearcher;
};
