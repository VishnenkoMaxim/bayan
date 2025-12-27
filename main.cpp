#include "bayan.h"
#include "CSearcher.h"
#include "CTimeMeasurer.h"

using namespace std;

int main(int argc, char **argv) {
    Settings settings;

    po::options_description desc("options");
    desc.add_options()  ("help,h", "Help info")
                        ("min-size,s", po::value<uint32_t>(&settings.min_file_size)->default_value(1), "Min file size, Mb")
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
        return 0;
    }

    if (vm.count("scan-dirs") <= 0){
        cout << "There are no dirs to look into. Terminate" << endl;
        return 1;
    }

    CSearcherBuilder searcher_builder;
    
    if (vm.count("exclude-dirs") > 0) {
        searcher_builder.withExcludedFolders(vm["exclude-dirs"].as<vector<string>>());
    }
    
    if (vm.count("mask") > 0){
        searcher_builder.withFilters(vm["mask"].as<vector<string>>());
    }

    if (vm.count("min-size") > 0)
    {
        settings.min_file_size = settings.min_file_size * _1MB;
    }
    
    searcher_builder.withBlockSize(settings.block_size);
    searcher_builder.withMinFileSize(settings.min_file_size);
    searcher_builder.withRecursive(settings.recursive);
    searcher_builder.withHashAlg(settings.hash_alg);
    searcher_builder.withFoldersToScan(vm["scan-dirs"].as<vector<string>>());
    auto searcher = searcher_builder.build();
    
    searcher.printSettings();

    // Обходим каталоги
    searcher.traverse();

    // Ищем дубликаты
    searcher.search();

    // todo send results
    const auto& results = searcher.getDuplicatedFiles();
    
    
    return 0;
}
