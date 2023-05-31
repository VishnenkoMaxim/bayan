#include "bayan.h"

bool CheckFilters(const fs::path &path, const vector<string> &_mask_vector){
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

vector<FileData> Traverse(fs::path& dir, const vector<string> &_mask_vector, const vector<string> &_excluded_folders, const Settings &_settings){
    vector<FileData> result;

    if (fs::is_directory(dir)){
        if (!_settings.recursive){
            for (auto& entry : boost::make_iterator_range(fs::directory_iterator(dir), {})){
                if (fs::is_regular_file(entry)){
                    if (fs::file_size(entry.path()) >= _settings.min_file_size && CheckFilters(entry.path(),_mask_vector)){
                        FileData _file;
                        _file.path = entry;
                        result.push_back(_file);
                    }
                }
            }
        } else {
            for (auto& entry : boost::make_iterator_range(fs::recursive_directory_iterator(dir), {})){
                if (fs::is_regular_file(entry)){
                    if (any_of(_excluded_folders.cbegin(), _excluded_folders.cend(), [&entry](const string &_dir){
                        if (entry.path().parent_path().string().find(_dir) != string::npos) return true;
                        return false;}))
                        continue;
                    if (fs::is_regular_file(entry)){
                        if (fs::file_size(entry.path()) >= _settings.min_file_size && CheckFilters(entry.path(),_mask_vector)) {
                            FileData _file;
                            _file.path = entry;
                            result.push_back(_file);
                        }
                    }
                }
            }
        }
    }
    return result;
}

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

uint32_t ReadBlockCRC(std::ifstream &_stream, char *tmp_buffer, uint32_t block_len, uint32_t &offset, uint32_t (*HashFunc)(const char*, const uint32_t)){
    _stream.read(tmp_buffer, block_len);

    if (_stream) {
        offset += block_len;
        return HashFunc(tmp_buffer, block_len);
    }
    memset(tmp_buffer + _stream.gcount(), 0, block_len - _stream.gcount());
    offset += _stream.gcount();
    return HashFunc(tmp_buffer, block_len);
}

vector<FileData> FindDuplicates(const unordered_multimap<uint32_t, FileData> &src){
    vector<FileData> duplicates;

    for (auto it=src.begin(); it != src.end(); it++){
        int cur_count = src.count(it->first);
        if (cur_count > 1){
            auto range = src.equal_range(it->first);
            vector<string> dupl;
            for_each(range.first, range.second, [&duplicates, &dupl](const auto &x){
                if (x.second.processed_bytes != fs::file_size(x.second.path)) duplicates.push_back(x.second);
                else dupl.push_back(x.second.path.string());
            });
            if (dupl.size() > 1) {
                for(const auto & d_it : dupl) cout << d_it << endl;
                cout << endl;
            }
            std::advance(it, cur_count-1);
        }
    }

    return duplicates;
}

void PrintSettings(const Settings &settings){
    cout << "recursive:" << settings.recursive << endl;
    cout << "min file size:" << settings.min_file_size << endl;
    cout << "block size:" << settings.block_size << endl;
    cout << "hash algorithm:" << settings.hash_alg << endl;
    cout << endl;
}
