//
// Created by robin on 7/22/16.
//

#include "SectorDbo.h"

Result<SectorDbo> SectorDbo::parse(std::string data)
{
    if (data.length() != 64) {
        return Result<SectorDbo>::error("Data length must be 64");
    }
    SectorDbo dbo;
    for (int i = 0; i < 4; ++i) {
        dbo._blocks[i] = data.substr(16 * i, 16);
    }
    return Result<SectorDbo>::ok(dbo);
}

const std::string &SectorDbo::getBlock(int block) const
{
    return _blocks[block];
}

std::string SectorDbo::getKeyA() const
{
    return _blocks[3].substr(0, 6);
}

std::string SectorDbo::getKeyB() const
{
    return _blocks[3].substr(10, 6);
}

std::string SectorDbo::getAccessBits() const
{
    return _blocks[3].substr(6, 4);
}
