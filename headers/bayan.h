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

#include "CHashReader.h"
#include "CDuplicatedFile.h"

namespace po = boost::program_options;
namespace fs = boost::filesystem;
namespace md5_hash = boost::uuids::detail;

#define _1MB (1024*1024)
#define _1GB (1024*1024*1024)

#define GUI_PORT 7698

struct Settings{
    uint32_t min_file_size;
    uint32_t block_size;
    bool recursive;
    std::string hash_alg;
};

struct FileData{
    fs::path path;
    uint32_t hash_block = 0;
    uint32_t processed_bytes = 0;
    bool err = false;
};

#endif //BAYAN_H
