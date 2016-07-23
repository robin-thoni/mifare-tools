//
// Created by robin on 7/22/16.
//

#include "FreeFareTagBusiness.h"

FreeFareTagBusiness::FreeFareTagBusiness(std::shared_ptr<FreeFareTag> tag)
    : _tag(tag)
{
}

ResultBool FreeFareTagBusiness::authenticate(int sector, int block, std::string key, int keyType)
{
    return _tag->authenticate(sector, block, key, keyType);
}

ResultString FreeFareTagBusiness::readBlock(int sector, int block, std::string key, int keyType)
{
    return _tag->readBlock(sector, block, key, keyType);
}

Result<SectorDbo> FreeFareTagBusiness::readSector(int sector, std::string key, int keyType)
{
    std::string res;
    int lastBlock = _tag->getSectorBlockCount(sector);
    for (int i = 0; i < lastBlock; ++i) {
        auto data = readBlock(sector, i, key, keyType);
        if (data) {
            res += data.getData();
        }
        else {
            return Result<SectorDbo>::error(data);
        }
    }
    return SectorDbo::parse(res);
}

const std::string &FreeFareTagBusiness::getUid() const
{
    return _tag->getUid();
}

freefare_tag_type FreeFareTagBusiness::getType() const
{
    return _tag->getType();
}

std::shared_ptr<FreeFareTag> FreeFareTagBusiness::getTag() const
{
    return _tag;
}

Result<std::vector<std::vector<std::pair<std::string, std::string>>>> FreeFareTagBusiness::mapKeys(std::vector<std::string> keys)
{
    std::vector<std::vector<std::pair<std::string, std::string>>> mappedKeys;

    for (int i = 0; i < 16; ++i) {
        std::vector<std::pair<std::string, std::string>> sectorKeys;
        for (int j = 0; j < 4; ++j) {
            std::pair<std::string, std::string> blockKeys;
            for (auto key : keys) {
                auto res = authenticate(i, j, key, MFC_KEY_A);
                if (res) {
                    blockKeys.first = key;
                }
                res = authenticate(i, j, key, MFC_KEY_B);
                if (res) {
                    blockKeys.second = key;
                }
            }
            sectorKeys.push_back(blockKeys);
        }
        mappedKeys.push_back(sectorKeys);
    }

    return Result<std::vector<std::vector<std::pair<std::string, std::string>>>>::ok(mappedKeys);
}