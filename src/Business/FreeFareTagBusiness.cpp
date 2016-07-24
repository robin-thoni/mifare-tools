//
// Created by robin on 7/22/16.
//

#include <DBO/StringUtils.h>
#include "FreeFareTagBusiness.h"

FreeFareTagBusiness::FreeFareTagBusiness(std::shared_ptr<FreeFareTag> tag)
    : _tag(tag)
{
}

ResultBool FreeFareTagBusiness::authenticate(int sector, std::string key, int keyType)
{
    return _tag->authenticate(sector, key, keyType);
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

Result<std::vector<SectorDbo>> FreeFareTagBusiness::read(MappedKeys keys, std::function<void(int, int)> cb)
{
    if (keys.size() != 16) {
        return Result<std::vector<SectorDbo>>::error("Must have 16 sectors keys");
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

Result<std::vector<SectorDbo>> FreeFareTagBusiness::read(std::vector<std::string> keys, std::function<void(int, int)> mapCb,
                                                         std::function<void(int, int)> readCb)
{
    auto mappedKeysResult = mapKeys(keys, mapCb);
    if (!mappedKeysResult) {
        return Result<std::vector<SectorDbo>>::error(mappedKeysResult);
    }
    return read(mappedKeysResult.getData(), readCb);
}

ResultBool FreeFareTagBusiness::writeBlock(int sector, int block, std::string key, int keyType, const std::string &data)
{
    return _tag->writeBlock(sector, block, key, keyType, StringUtils::ensureSize(data, 16));
}

ResultBool FreeFareTagBusiness::writeSector(int sector, std::string key, int keyType, const std::string &data)
{
    std::string d = StringUtils::ensureSize(data, 64);
    std::string errors;
    for (int b = 0; b < 4; ++b) {
        auto result = writeBlock(sector, b, key, keyType, d.substr(b * 16, 16));
        if (!result) {
            errors += std::string(errors.empty() ? "" : "\n") + "Failed to write sector " + std::to_string(sector) +
                      " block " + std::to_string(b) + " with key " + (keyType == MFC_KEY_A ? "A" : "B") + ": " + result.getError();
        }
    }
    if (errors.empty()) {
        return ResultBool::ok(true);
    }
    return ResultBool::error(errors);
}

ResultBool FreeFareTagBusiness::write(MappedKeys keys, const std::string &data, bool writeSector0, std::function<void(int, int)> cb)
{
    if (keys.size() != 16) {
        return ResultBool::error("Must have 16 sectors keys");
    }
    std::string d = StringUtils::ensureSize(data, 1024);
    std::string errors;
    int done = 0;
    int total = 4 * (keys.size() - (writeSector0 ? 0 : 1));
    for (int s = writeSector0 ? 0 : 1; s < keys.size(); ++s) {
        auto sectorKey = keys[s];
        for (int b = 0; b < 4; ++b) {
            std::string blockData = d.substr((s * 64) + (b * 16), 16);
            if (cb != 0 && done < total) {
                bool keyA = false;
                bool keyB = false;
                std::string sectorErrors;
                if (!sectorKey.first.empty()) {
                    auto resultA = writeBlock(s, b, sectorKey.first, MFC_KEY_A, blockData);
                    if (resultA) {
                        keyA = true;
                    }
                    else {
                        sectorErrors = "Failed to write sector " + std::to_string(s) +
                                       " block " + std::to_string(b) + " with key A: " + resultA.getError();
                    }
                }
                if (!keyA && !sectorKey.second.empty()) {
                    auto resultB = writeBlock(s, b, sectorKey.first, MFC_KEY_A, blockData);
                    if (resultB) {
                        keyB = true;
                    }
                    else {
                        sectorErrors = std::string(sectorErrors.empty() ? "" : "\n") + "Failed to write sector " + std::to_string(s) +
                                       " block " + std::to_string(b) + " with key B: " + resultB.getError();
                    }
                }
                if (!keyA && !keyB) {
                    errors += std::string(errors.empty() ? "" : "\n") + sectorErrors;
                }
                cb(++done, total);
            }
        }
    }
    if (cb != 0 && done < total) {
        cb(total, total);
    }
    if (errors.empty()) {
        return ResultBool::ok(true);
    }
    return ResultBool::error(errors);
}

ResultBool FreeFareTagBusiness::write(std::vector<std::string> keys, const std::string &data, bool writeSector0,
                                      std::function<void(int, int)> mapCb, std::function<void(int, int)> writeCb)
{
    auto mappedKeysResult = mapKeys(keys, mapCb);
    if (!mappedKeysResult) {
        return ResultBool::error(mappedKeysResult);
    }
    return write(mappedKeysResult.getData(), data, writeSector0, writeCb);
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
