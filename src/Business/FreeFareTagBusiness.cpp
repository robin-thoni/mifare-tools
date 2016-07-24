//
// Created by robin on 7/22/16.
//

#include "FreeFareTagBusiness.h"

FreeFareTagBusiness::FreeFareTagBusiness(std::shared_ptr<FreeFareTag> tag)
    : _tag(tag)
{
}

ResultBool FreeFareTagBusiness::authenticate(int sector, std::string key, int keyType)
{
    return _tag->authenticate(sector, key, keyType);
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
    MappedKeys mappedKeys;
    int done = 0;
    int total = 16 * keys.size();

    for (int i = 0; i < 16; ++i) {
        std::pair<std::string, std::string> blockKeys;
        for (int k = 0; k < keys.size(); ++k) {
            auto key = keys[k];
            if (authenticate(i, key, MFC_KEY_A)) {
                blockKeys.first = key;
            }
            if (authenticate(i, key, MFC_KEY_B)) {
                blockKeys.second = key;
            }
            if (cb != 0) {
                cb(++done, total);
            }
            if (!blockKeys.first.empty() && !blockKeys.second.empty()) {
                break;
            }
        }
        mappedKeys.push_back(blockKeys);
    }
    if (cb != 0 && done < total) {
        cb(total, total);
    }

    return Result<MappedKeys>::ok(mappedKeys);
}

Result<std::vector<SectorDbo>> FreeFareTagBusiness::dump(MappedKeys keys, std::function<void(int, int)> cb)
{
    if (keys.size() != 16) {
        return Result<std::vector<SectorDbo>>::error("Must have 16 sectors");
    }
    int done = 0;
    int total = 4 * keys.size();
    std::vector<SectorDbo> sectors;
    for (int s = 0; s < keys.size(); ++s) {
        auto sectorKey = keys[s];
        SectorDbo sector;
        bool keyA = false;
        bool keyB = false;
        for (int b = 0; b < 3; ++b) {
            std::string data = "";
            if (!sectorKey.first.empty()) {
                auto blockResult = readBlock(s, b, sectorKey.first, MFC_KEY_A);
                if (blockResult) {
                    data = blockResult.getData();
                    keyA = true;
                }
            }
            if (!sectorKey.second.empty()) {
                auto blockResult = readBlock(s, b, sectorKey.second, MFC_KEY_B);
                if (blockResult) {
                    if (data.empty()) {
                        data = blockResult.getData();
                    }
                    keyB = true;
                }
            }
            sector.setBlock(b, data);
            if (cb != 0) {
                cb(++done, total);
            }
        }
        int b = 3;
        std::string dataA = "";
        std::string dataB = "";
        if (!sectorKey.first.empty()) {
            auto blockResult = readBlock(s, b, sectorKey.first, MFC_KEY_A);
            if (blockResult) {
                dataA = blockResult.getData();
                keyA = true;
            }
        }
        if (!sectorKey.second.empty()) {
            auto blockResult = readBlock(s, b, sectorKey.second, MFC_KEY_B);
            if (blockResult) {
                dataB = blockResult.getData();
                keyB = true;
            }
        }
        if (cb != 0) {
            cb(++done, total);
        }

        sector.setBlock(b, dataA);
        AccessBitsDbo accessBitsDboA = sector.getAccessBitsDbo();
        sector.setBlock(b, dataB);
        AccessBitsDbo accessBitsDboB = sector.getAccessBitsDbo();
        sector.setKeyA(keyA ? sectorKey.first : "");
        sector.setKeyB(keyB ? sectorKey.second : "");

        std::string accessBits;
        if (keyA && accessBitsDboA.canKeyAReadAccessBitsTrailer()) {
            accessBits = accessBitsDboA.getBits();
        }
        else if (keyB && accessBitsDboB.canKeyBReadAccessBitsTrailer()) {
            accessBits = accessBitsDboB.getBits();
        }
        sector.setAccessBits(accessBits);

        sectors.push_back(sector);
    }
    if (cb != 0 && done < total) {
        cb(total, total);
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
