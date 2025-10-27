#include "bayan.h"

#include <chrono>
class CTimeMeasurer {
public:
    CTimeMeasurer() {
        start_time = chrono::system_clock::now();
    }

    void start() {
        start_time = chrono::system_clock::now();
    }

    void stop() {
        stop_time = chrono::system_clock::now();

        auto seconds = std::chrono::duration_cast<std::chrono::seconds>(stop_time-start_time).count();
        auto m_sec = std::chrono::duration_cast<std::chrono::milliseconds>(stop_time-start_time).count() - seconds*100;
        cout << "Total elapsed time: " << seconds << "." << m_sec << " sec" << endl;
    }

private:
    chrono::system_clock::time_point start_time;
    chrono::system_clock::time_point stop_time;
};

uint64_t duplicated_memory = 0;

int main(int argc, char **argv) {
    Settings settings;

    po::options_description desc("options");
    desc.add_options()  ("help,h", "Help info")
                        ("min-size,s", po::value<uint32_t>(&settings.min_file_size)->default_value(1), "Min file size, bytes")
                        ("block-size,b", po::value<uint32_t>(&settings.block_size)->default_value(16384), "Reading block size")
                        ("scan-dirs,d", po::value<vector<string>>()->multitoken(), "Directories to scan")
                        ("exclude-dirs,e", po::value<vector<string>>()->multitoken(), "Excluded directories")
                        ("recursive,r", po::value<bool>(&settings.recursive)->default_value(false)->zero_tokens(), "Make it recursively")
                        ("hash-alg,a", po::value<string>(&settings.hash_alg)->default_value("crc32"), "Hash algorithm")
                        ("mask,m", po::value<vector<string>>()->multitoken(), "Regular expression mask");
                

    po::variables_map vm;
    po::store(parse_command_line(argc, argv, desc), vm);
    po::notify(vm);

    if (vm.count("help")) {
        cout << desc << endl;
    }

    if (vm.count("scan-dirs") <= 0){
        cout << "There are no dirs to look into. Terminate" << endl;
        return 1;
    }

    vector<string> excluded_folders;
    if (vm.count("exclude-dirs") > 0) {
        excluded_folders = vm["exclude-dirs"].as<vector<string>>();
    }

    vector<string> filters;
    if (vm.count("mask") > 0){
        filters = vm["mask"].as<vector<string>>();
    }
    PrintSettings(settings);

    uint32_t (*HashFunc)(const char*, const uint32_t);
    if (settings.hash_alg == "md5" || settings.hash_alg == "MD5") HashFunc = MD5;
    else HashFunc = CRC32;
    
    cout << "Traversing... " << endl;
    vector<FileData> all_files;
    for(const auto &it : vm["scan-dirs"].as<vector<string>>()){
        fs::path p(it);
        auto paths = Traverse(p, filters, excluded_folders, settings);
        all_files.insert(all_files.end(), paths.begin(), paths.end());
    }
    cout << "files amount to compare: " << all_files.size() << endl;

    cout << "Comparing file data..." << endl << endl;
    unordered_multimap<uint32_t, FileData> data;
    
    CTimeMeasurer m;
    CHashReader hash_reader(settings.block_size, HashFunc);
    while(!all_files.empty()){
        data.clear();
        hash_reader.init();
        
        for(auto &it : all_files) {
            hash_reader.addTask({it.path, it.processed_bytes, it.hash_block, it.err});
        }
        hash_reader.wait();

        for (auto& it : all_files) {
            if (!it.err) {
                data.insert(make_pair(it.hash_block, it));
            }
        }

        all_files = FindDuplicates(data);
    }
    m.stop();
    if (duplicated_memory < 1024)
    {
        cout << "Duplicated memory size: " << duplicated_memory << "." << duplicated_memory % 1024 << " MB" << endl;
    }
    else
    {
        cout << "Duplicated memory size: " << duplicated_memory / 1024 << "." << duplicated_memory % 1024 << " GB" << endl;
    }
    
    
    return 0;
}
