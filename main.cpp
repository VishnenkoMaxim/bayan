#include "bayan.h"

int main(int argc, char **argv) {
    Settings settings;

    po::options_description desc("options");
    desc.add_options()  ("help,h", "Help info")
                        ("min-size,s", po::value<uint32_t>(&settings.min_file_size)->default_value(1), "Min file size, bytes")
                        ("block-size,b", po::value<uint32_t>(&settings.block_size)->default_value(256), "Reading block size")
                        ("scan-dirs,d", po::value<vector<string>>()->multitoken(), "Directories to scan")
                        ("exclude-dirs,e", po::value<vector<string>>()->multitoken(), "Excluded directories")
                        ("recursive,r", po::value<bool>(&settings.recursive)->default_value(false)->zero_tokens(), "Make it recursively")
                        ("hash-alg,a", po::value<string>(&settings.hash_alg)->default_value("crc32"), "Hash algorithm")
                        ("mask,m", po::value<vector<string>>()->multitoken(), "Regular expression mask");

    po::variables_map vm;
    po::store(parse_command_line(argc, argv, desc), vm);
    po::notify(vm);

    if (vm.count("help")) cout << desc << endl;

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
    char *buf = new char[settings.block_size];
    std::ifstream file;

    cout << "Comparing file data..." << endl << endl;
    unordered_multimap<uint32_t, FileData> data;
    while(!all_files.empty()){
        data.clear();
        for(auto &it : all_files){
            file.open(it.path, std::ifstream::binary);
            if (file.good()){
                file.seekg(it.processed_bytes);
                it.hash_block = ReadBlockCRC(file, buf, settings.block_size, it.processed_bytes, HashFunc);
                file.close();
                data.insert(make_pair(it.hash_block, it));
            } else cout << "open error: " <<  it.path.string() << endl;
        }
        all_files = FindDuplicates(data);
    }

    delete[] buf;
    return 0;
}
