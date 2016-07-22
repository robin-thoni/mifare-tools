//
// Created by robin on 6/19/16.
//

#include <sstream>
#include <map>
#include "FreeFareTag.h"

FreeFareTag::FreeFareTag(FreefareTag tag)
    : _tag(tag)
{
    _type = freefare_get_tag_type(tag);
    _uid = freefare_get_tag_uid(tag);
}

FreeFareTag::~FreeFareTag()
{
    freefare_free_tag(_tag);
}

ResultBool FreeFareTag::authenticate(int sector, int block, std::string key, int keyType)
{
    if (mifare_classic_connect(_tag) != 0) {
        return ResultBool::error("Failed to connect to MIFARE tag");
    }

    block = mifare_classic_sector_first_block((MifareClassicBlockNumber)sector) + block;
    if (mifare_classic_authenticate(_tag, (MifareClassicBlockNumber)block, (const unsigned char*)key.c_str(),
                                    (MifareClassicKeyType)keyType) != 0) {
        return ResultBool::error("Failed to authenticate to MIFARE tag");
    }

    mifare_classic_disconnect(_tag);
    return ResultBool::ok(true);
}

ResultString FreeFareTag::readBlock(int sector, int block, std::string key, int keyType)
{
    if (mifare_classic_connect(_tag) != 0) {
        return ResultString::error("Failed to connect to MIFARE tag");
    }

    block = mifare_classic_sector_first_block((MifareClassicBlockNumber)sector) + block;
    if (mifare_classic_authenticate(_tag, (MifareClassicBlockNumber)block, (const unsigned char*)key.c_str(),
                                    (MifareClassicKeyType)keyType) != 0) {
        return ResultString::error("Failed to authenticate to MIFARE tag");
    }

    MifareClassicBlock data;
    if (mifare_classic_read(_tag, (MifareClassicBlockNumber)block, &data)) {
        return ResultString::error("Failed to read MIFARE tag data");
    }

    mifare_classic_disconnect(_tag);
    return ResultString::ok(std::string((const char*)data, sizeof(data)));
}

const freefare_tag *FreeFareTag::getTag() const
{
    return _tag;
}

int FreeFareTag::getFirstBlock(int sector) const
{
    return mifare_classic_sector_first_block(sector);
}

int FreeFareTag::getLastBlock(int sector) const
{
    return mifare_classic_sector_last_block(sector);
}

int FreeFareTag::getSectorBlockCount(int sector)
{
    return mifare_classic_sector_block_count(sector);
}

freefare_tag_type FreeFareTag::getType() const
{
    return _type;
}

const std::string &FreeFareTag::getUid() const
{
    return _uid;
}
