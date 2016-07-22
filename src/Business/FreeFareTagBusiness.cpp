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

ResultString FreeFareTagBusiness::readSector(int sector, std::string key, int keyType)
{
    std::string res;
    int lastBlock = _tag->getSectorBlockCount(sector);
    for (int i = 0; i < lastBlock; ++i) {
        auto data = readBlock(sector, i, key, keyType);
        if (data) {
            res += data.getData();
        }
        else {
            return data;
        }
    }
    return ResultString::ok(res);
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