//
// Created by robin on 6/19/16.
//

#ifndef MIFARE_TOOLS_FREEFARETAG_H
#define MIFARE_TOOLS_FREEFARETAG_H

#include <freefare.h>
#include <string>
#include <DBO/Result.h>

class FreeFareTag
{
public:

    FreeFareTag(FreefareTag tag);
    ~FreeFareTag();

    ResultBool authenticate(int sector, std::string key, int keyType);

    ResultString readBlock(int sector, int block, std::string key, int keyType);

    ResultBool writeBlock(int sector, int block, std::string key, int keyType, const std::string& data);

    int getFirstBlock(int sector) const;

    int getLastBlock(int sector) const;

    int getSectorBlockCount(int sector);

    const freefare_tag * getTag() const;

    freefare_tag_type getType() const;

    const std::string & getUid() const;

private:
    FreefareTag _tag;

    freefare_tag_type _type;

    std::string _uid;
};


#endif //MIFARE_TOOLS_FREEFARETAG_H
