//
// Created by vimax on 10.04.2026.
//

#pragma once

#include <vector>

#include "CDuplicatedFile.h"

class IDataTransformer {
public:

    virtual std::string transform(const std::vector<CDuplicatedFile>& data) = 0;
    virtual ~IDataTransformer() = default;
};

class CJSONDataTransformer : public IDataTransformer
{
public:
    std::string transform(const std::vector<CDuplicatedFile>& data) override;
};

class CRawDataTransformer : public IDataTransformer
{
public:
    std::string transform(const std::vector<CDuplicatedFile>& data) override;
};