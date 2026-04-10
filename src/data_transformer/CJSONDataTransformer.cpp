//
// Created by vimax on 10.04.2026.
//
#include <iostream>
#include <boost/json.hpp>

#include "data_transformer/IDatatrnsformer.h"

std::string CJSONDataTransformer::transform(const std::vector<CDuplicatedFile>& data)
{
    if (data.empty())
    {
        return {};
    }

    std::vector<char> transformed_data;
    std::string json_str;

    try
    {
        boost::json::array arr;
        for (const auto& file : data)
        {
            boost::json::object obj;
            obj["size"] = file.mSize;
            boost::json::array names;

            for (const auto& path : file.mPathList)
            {
                names.emplace_back(path.string());
            }
            obj["names"] = std::move(names);
            arr.emplace_back(std::move(obj));
        }
        boost::json::object root;
        root["duplicated files"] = std::move(arr);

        json_str = boost::json::serialize(root);
    }
    catch (const boost::system::system_error& e)
    {
        std::cerr << "Boost system error while building JSON: " << e.what() << std::endl;
        return {};
    }
    catch (const std::exception& e)
    {
        std::cerr << "Standard exception while building JSON: " << e.what() << std::endl;
        return {};
    }

    return json_str;
}