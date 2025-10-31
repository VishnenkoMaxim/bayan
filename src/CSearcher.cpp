//
// Created by vimax on 30.10.25.
//

#include "CSearcher.h"

using namespace std;

uint32_t CRC32(const char* data, const uint32_t data_len){
    boost::crc_32_type crc_result;
    crc_result.process_bytes(data, data_len);
    return crc_result.checksum();
}

uint32_t MD5(const char* data, const uint32_t data_len){
    md5_hash::md5 hash;
    md5_hash::md5::digest_type digest;

    hash.process_bytes(data, data_len);
    hash.get_digest(digest);

    uint32_t res_hash;
    memcpy(&res_hash, digest, 4);
    return res_hash;
}

CSearcherBuilder& CSearcherBuilder::withFilters(const std::vector<std::string>& filters)
{
    mSearcher.mFilters = filters;
    return *this;
}

CSearcherBuilder& CSearcherBuilder::withMinFileSize(uint32_t min_file_size)
{
    mSearcher.mMinFileSize = min_file_size;
    return *this;
}

CSearcherBuilder& CSearcherBuilder::withBlockSize(uint32_t block_size)
{
    mSearcher.mBlockSize = block_size;
    return *this;
}

CSearcherBuilder& CSearcherBuilder::withRecursive(const bool is_recursive)
{
    mSearcher.isRecursive = is_recursive;
    return *this;
}

CSearcherBuilder& CSearcherBuilder::withHashAlg(const std::string& hash_alg)
{
    mSearcher.mHashAlg = hash_alg;
    return *this;
}

CSearcherBuilder& CSearcherBuilder::withExcludedFolders(const std::vector<std::string>& excluded_folders)
{
    mSearcher.mExcludedFolders = excluded_folders;
    return *this;
}

CSearcherBuilder& CSearcherBuilder::withFoldersToScan(const std::vector<std::string>& scan_folders)
{
    mSearcher.mScanFolders = scan_folders;
    return *this;
}

CSearcher& CSearcherBuilder::build()
{
    return mSearcher;
}

bool CSearcher::checkFilters(const fs::path &path, const std::vector<std::string> &_mask_vector)
{
    if (!_mask_vector.empty()){
        boost::smatch match_res;
        bool all_match = true;
        for(const auto &it_mask : _mask_vector){
            boost::regex filter(it_mask);
            if (!boost::regex_match(path.string(), match_res, filter)){
                all_match = false;
                break;
            }
        }
        if (all_match) return true;
        return false;
    }
    return true;
}

std::vector<FileData> CSearcher::traverseDir(const fs::path& dir) const
{
    vector<FileData> result;

    if (fs::is_directory(dir)){
        if (!isRecursive){
            for (auto& entry : boost::make_iterator_range(fs::directory_iterator(dir), {})){
                if (fs::is_regular_file(entry)){
                    if (fs::file_size(entry.path()) >= mMinFileSize && checkFilters(entry.path(), mFilters)){
                        FileData _file;
                        _file.path = entry;
                        result.emplace_back(_file);
                    }
                }
            }
        } else {
            for (auto& entry : boost::make_iterator_range(fs::recursive_directory_iterator(dir), {})){
                if (fs::is_regular_file(entry)){
                    if (any_of(mExcludedFolders.cbegin(), mExcludedFolders.cend(), [&entry](const string &_dir){
                        if (entry.path().parent_path().string().find(_dir) != string::npos) return true;
                        return false;}))
                        continue;
                    if (fs::is_regular_file(entry)){
                        if (fs::file_size(entry.path()) >= mMinFileSize && checkFilters(entry.path(), mFilters)) {
                            FileData _file;
                            _file.path = entry;
                            result.emplace_back(_file);
                        }
                    }
                }
            }
        }
    }
    return result;
}

std::vector<FileData> CSearcher::FindDuplicates(const std::unordered_multimap<uint32_t, FileData> &src, std::vector<CDuplicatedFile>& duplicated_files) const
{
    vector<FileData> duplicates;

    for (auto it=src.begin(); it != src.end(); ++it){
        const auto cur_count = src.count(it->first);
        if (cur_count > 1){
            auto range = src.equal_range(it->first);
            vector<std::pair<boost::filesystem::path, size_t>> dupl;
            for_each(range.first, range.second, [&duplicates, &dupl](const auto &x){
                if (x.second.processed_bytes != fs::file_size(x.second.path))
                {
                    duplicates.emplace_back(x.second);
                }
                else
                {
                    dupl.emplace_back(make_pair(x.second.path.string(), x.second.processed_bytes));
                }
            });
            if (dupl.size() > 1) {
                CDuplicatedFile duplicated_file(dupl[0].second, dupl[0].first.filename().string());
                
                for(const auto& dup_file : dupl)
                {
                    duplicated_file.addPath(dup_file.first);
                }
                duplicated_files.emplace_back(std::move(duplicated_file));
            }
            std::advance(it, cur_count-1);
        }
    }

    return duplicates;
}

void CSearcher::printSettings() const
{
    cout << "recursive:" << isRecursive << endl;
    cout << "min file size:" << mMinFileSize << endl;
    cout << "block size:" << mBlockSize << endl;
    cout << "hash algorithm:" << mHashAlg << endl;
    cout << endl;
}


void CSearcher::traverse()
{
    mTimeMeasurer.start();
    std::cout << "Traversing... " << std::endl;
    
    for(const auto &it : mScanFolders){
        fs::path p(it);
        auto paths = traverseDir(p);
        mAllFiles.insert(mAllFiles.end(), paths.begin(), paths.end());
    }
    cout << "files amount to compare: " << mAllFiles.size() << endl;
}

void CSearcher::search()
{
    uint32_t (*HashFunc)(const char*, const uint32_t);
    if (mHashAlg == "md5" || mHashAlg == "MD5") HashFunc = MD5;
    else HashFunc = CRC32;

    cout << "Comparing file data..." << endl << endl;
    unordered_multimap<uint32_t, FileData> data;
    
    CHashReader hash_reader(mBlockSize, HashFunc);
    while(!mAllFiles.empty()){
        data.clear();
        hash_reader.init();
        
        for(auto &it : mAllFiles) {
            hash_reader.addTask({it.path, it.processed_bytes, it.hash_block, it.err});
        }
        hash_reader.wait();

        for (auto& it : mAllFiles) {
            if (!it.err) {
                data.insert(make_pair(it.hash_block, it));
            }
        }

        mAllFiles = FindDuplicates(data, mDuplicatedFiles);
    }
    mTimeMeasurer.stop();
    
    if (!mDuplicatedFiles.empty())
    {
        uint64_t duplicated_memory = 0;
        for (auto &dup_file : mDuplicatedFiles)
        {
            dup_file.print();
            duplicated_memory += dup_file.getPotentialFreeSize();
        }
        mTimeMeasurer.print();
        cout << "Duplicated files count: " << mDuplicatedFiles.size() << endl;
        
        if (duplicated_memory < _1GB)   cout << "Duplicated memory size: " << duplicated_memory / _1MB << "." << duplicated_memory % _1MB << " MB" << endl;
        else                            cout << "Duplicated memory size: " << duplicated_memory / _1GB << "." << duplicated_memory % _1GB << " GB" << endl;
        return;
    }
    cout << "There is no duplicated files in source dirs" << endl;
    
}