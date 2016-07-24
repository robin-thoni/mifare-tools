//
// Created by robin on 7/22/16.
//

#ifndef MIFARE_TOOLS_FREEFARETAGBUSINESS_H
#define MIFARE_TOOLS_FREEFARETAGBUSINESS_H


#include <boost/shared_ptr.hpp>
#include <DataAccess/FreeFareTag.h>
#include <DataAccess/SectorDbo.h>

typedef std::vector<std::pair<std::string, std::string>> MappedKeys;

class FreeFareTagBusiness
{
public:
    FreeFareTagBusiness(std::shared_ptr<FreeFareTag> tag);

    ResultBool authenticate(int sector, std::string key, int keyType);

    Result<MappedKeys> mapKeys(std::vector<std::string> keys, std::function<void(int, int)> cb = 0);

    ResultString readBlock(int sector, int block, std::string key, int keyType);

    Result<SectorDbo> readSector(int sector, std::string key, int keyType);

    Result<std::vector<SectorDbo>> read(MappedKeys keys, std::function<void(int, int)> cb = 0);

    Result<std::vector<SectorDbo>> read(std::vector<std::string> keys, std::function<void(int, int)> mapCb = 0,
                                        std::function<void(int, int)> dumpCb = 0);

    ResultBool writeBlock(int sector, int block, std::string key, int keyType, const std::string& data);

    ResultBool writeSector(int sector, std::string key, int keyType, const std::string& data);

    ResultBool write(MappedKeys keys, const std::string& data, bool writeSector0, std::function<void(int, int)> cb = 0);

    ResultBool write(std::vector<std::string> keys, const std::string& data, bool writeSector0,
                     std::function<void(int, int)> mapCb = 0, std::function<void(int, int)> dumpCb = 0);

    const std::string& getUid() const;

    freefare_tag_type getType() const;

    std::shared_ptr<FreeFareTag> getTag() const;

protected:
    std::shared_ptr<FreeFareTag> _tag;
};


#endif //MIFARE_TOOLS_FREEFARETAGBUSINESS_H
