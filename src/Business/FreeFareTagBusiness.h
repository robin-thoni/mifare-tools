//
// Created by robin on 7/22/16.
//

#ifndef MIFARE_TOOLS_FREEFARETAGBUSINESS_H
#define MIFARE_TOOLS_FREEFARETAGBUSINESS_H


#include <boost/shared_ptr.hpp>
#include <DataAccess/FreeFareTag.h>

class FreeFareTagBusiness
{
public:
    FreeFareTagBusiness(std::shared_ptr<FreeFareTag> tag);

    ResultBool authenticate(int sector, int block, std::string key, int keyType);

    ResultString readBlock(int sector, int block, std::string key, int keyType);

    ResultString readSector(int sector, std::string key, int keyType);

    const std::string & getUid() const;

    freefare_tag_type getType() const;

    std::shared_ptr<FreeFareTag> getTag() const;

protected:
    std::shared_ptr<FreeFareTag> _tag;
};


#endif //MIFARE_TOOLS_FREEFARETAGBUSINESS_H
