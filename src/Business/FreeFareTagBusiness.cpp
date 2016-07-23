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
    return Result<SectorDbo>::ok(SectorDbo(res));
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

Result<MappedKeys> FreeFareTagBusiness::mapKeys(std::vector<std::string> keys, std::function<void(int, int)> cb)
{
    std::vector<std::vector<std::pair<std::string, std::string>>> mappedKeys;

    for (int i = 0; i < 16; ++i) {
        std::vector<std::pair<std::string, std::string>> sectorKeys;
        for (int j = 0; j < 4; ++j) {
            std::pair<std::string, std::string> blockKeys;
            for (int k = 0; k < keys.size(); ++k) {
                auto key = keys[k];
                if (authenticate(i, j, key, MFC_KEY_A)) {
                    blockKeys.first = key;
                }
                if (authenticate(i, j, key, MFC_KEY_B)) {
                    blockKeys.second = key;
                }
                if (cb != 0) {
                    cb((i * 4 * keys.size()) + (j * keys.size()) + k + 1, 16 * 4 * keys.size());
                }
                if (!blockKeys.first.empty() && !blockKeys.second.empty()) {
                    break;
                }
            }
            sectorKeys.push_back(blockKeys);
        }
        mappedKeys.push_back(sectorKeys);
    }

    return Result<std::vector<std::vector<std::pair<std::string, std::string>>>>::ok(mappedKeys);
}

Result<std::vector<SectorDbo>> FreeFareTagBusiness::dump(MappedKeys keys, std::function<void(int, int)> cb)
{
    if (keys.size() != 16) {
        return Result<std::vector<SectorDbo>>::error("Must have 16 sectors");
    }
    for (int i = 0; i < keys.size(); ++i) {
        auto key = keys[i];
        if (key.size() != 4) {
            return Result<std::vector<SectorDbo>>::error("Must have 4 keys in sector " + i);
        }
    }
    std::vector<SectorDbo> sectors;
    for (int s = 0; s < keys.size(); ++s) {
        auto sectorKeys = keys[s];
        SectorDbo sector;
        bool keyA = false;
        bool keyB = false;
        for (int b = 0; b < sectorKeys.size() - 1; ++b) {
            auto blockKey = sectorKeys[b];
            std::string data = "";
            if (!blockKey.first.empty()) {
                auto blockResult = readBlock(s, b, blockKey.first, MFC_KEY_A);
                if (blockResult) {
                    data = blockResult.getData();
                    keyA = true;
                }
            }
            if (!blockKey.second.empty()) {
                auto blockResult = readBlock(s, b, blockKey.second, MFC_KEY_B);
                if (blockResult) {
                    data = blockResult.getData();
                    keyB = true;
                }
            }
            sector.setBlock(b, data);
            if (cb != 0) {
                cb((s * sectorKeys.size()) + b + 1, keys.size() * sectorKeys.size());
            }
        }
        int b = sectorKeys.size() - 1;
        auto blockKey = sectorKeys[b];
        std::string data = "";
        if (!blockKey.first.empty()) {
            auto blockResult = readBlock(s, b, blockKey.first, MFC_KEY_A);
            if (blockResult) {
                data = blockResult.getData();
                keyA = true;
            }
        }
        if (!blockKey.second.empty()) {
            auto blockResult = readBlock(s, b, blockKey.second, MFC_KEY_B);
            if (blockResult) {
                data = blockResult.getData();
                keyB = true;
            }
        }
        sector.setBlock(b, data);
        AccessBitsDbo accessBitsDbo = sector.getAccessBitsDbo();
        if (cb != 0) {
            cb((s * sectorKeys.size()) + b + 1, keys.size() * sectorKeys.size());
        }

        sector.setKeyA(keyA ? blockKey.first : "");
        sector.setKeyB(keyB ? blockKey.second : "");
        std::string accessBits;
        if (!data.empty())
        {
            if ((keyA && accessBitsDbo.canKeyAReadAccessBitsTrailer()) || (keyB && accessBitsDbo.canKeyBReadAccessBitsTrailer())) {
                accessBits = accessBitsDbo.getBits();
            }
        }
        sector.setAccessBits(accessBits);

        sectors.push_back(sector);
    }

    return Result<std::vector<SectorDbo>>::ok(sectors);
}

Result<std::vector<SectorDbo>> FreeFareTagBusiness::dump(std::vector<std::string> keys, std::function<void(int, int)> mapCb, std::function<void(int, int)> dumpCb)
{
    auto mappedKeysResult = mapKeys(keys, mapCb);
    if (!mappedKeysResult) {
        return Result<std::vector<SectorDbo>>::error(mappedKeysResult);
    }
    return dump(mappedKeysResult.getData(), dumpCb);
}
