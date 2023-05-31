#ifndef BAYAN_H
#define BAYAN_H

#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include <cstring>
#include <algorithm>

#include <stdio.h>
#include <stdlib.h>

#include <boost/program_options.hpp>
#include <boost/filesystem.hpp>
#include <boost/range/iterator_range.hpp>
#include <boost/regex.hpp>
#include <boost/crc.hpp>
#include <boost/uuid/detail/md5.hpp>
#include <fstream>

namespace po = boost::program_options;
namespace fs = boost::filesystem;
namespace md5_hash = boost::uuids::detail;

using namespace std;

struct Settings{
    uint32_t min_file_size;
    uint32_t block_size;
    bool recursive;
    string hash_alg;
};

struct FileData{
    fs::path path;
    uint32_t hash_block;
    uint32_t processed_bytes;

    FileData() : hash_block(0), processed_bytes(0){};
};

bool CheckFilters(const fs::path &path, const vector<string> &_mask_vector);
vector<FileData> Traverse(fs::path& dir, const vector<string> &_mask_vector, const vector<string> &_excluded_folders, const Settings &_settings);
uint32_t CRC32(const char* data, uint32_t data_len);
uint32_t MD5(const char* data, uint32_t data_len);
uint32_t ReadBlockCRC(std::ifstream &_stream, char *tmp_buffer, uint32_t block_len, uint32_t &offset, uint32_t (*HashFunc)(const char*, const uint32_t));
vector<FileData> FindDuplicates(const unordered_multimap<uint32_t, FileData> &src);
void PrintSettings(const Settings &settings);

#endif //BAYAN_H
