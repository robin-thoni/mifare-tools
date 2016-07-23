//
// Created by robin on 7/22/16.
//

#include <DBO/StringUtils.h>
#include "SectorDbo.h"

SectorDbo::SectorDbo(const std::string &data)
{
    std::string d = StringUtils::ensureSize(data, 64);
    for (int i = 0; i < 4; ++i) {
        _blocks[i] = d.substr(16 * i, 16);
        _haveBlocks[i] = true;
    }
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

AccessBitsDbo SectorDbo::getAccessBitsDbo() const
{
    return AccessBitsDbo(getAccessBits());
}

void SectorDbo::setBlock(int block, const std::string &data)
{
    std::string d = StringUtils::ensureSize(data, 16);
    if (block == 3) {
        setKeyA(d.substr(0, 6));
        setKeyB(d.substr(10, 6));
        setAccessBits(d.substr(6, 4));
    }
    else {
        _haveBlocks[block] = !data.empty();
        _blocks[block] = d;
    }
}

void SectorDbo::setKeyA(const std::string &key)
{
    std::string k = StringUtils::ensureSize(key, 6);
    for (int i = 0; i < k.size(); ++i) {
        _blocks[3][i] = k[i];
    }
    _hasKeyA = !key.empty();
}

void SectorDbo::setKeyB(const std::string &key)
{
    std::string k = StringUtils::ensureSize(key, 6);
    for (int i = 0; i < k.size(); ++i) {
        _blocks[3][10 + i] = k[i];
    }
    _hasKeyB = !key.empty();
}

void SectorDbo::setAccessBits(const std::string &accessBits)
{
    std::string a = StringUtils::ensureSize(accessBits, 4);
    for (int i = 0; i < a.size(); ++i) {
        _blocks[3][6 + i] = a[i];
    }
    _hasAccessBits = !accessBits.empty();
}

void SectorDbo::setAccessBits(const AccessBitsDbo &accessBits)
{
    setAccessBits(accessBits.getBits());
}

bool SectorDbo::hasBlock(int block) const
{
    if (block == 3) {
        return _hasKeyA && _hasKeyB && _hasAccessBits;
    }
    return _haveBlocks[block];
}

bool SectorDbo::hasKeyA() const
{
    return _hasKeyA;
}

bool SectorDbo::hasKeyB() const
{
    return _hasKeyB;
}

bool SectorDbo::hasAccessBits() const
{
    return _hasAccessBits;
}
