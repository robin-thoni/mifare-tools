//
// Created by robin on 7/22/16.
//

#ifndef MIFARE_TOOLS_SECTORDBO_H
#define MIFARE_TOOLS_SECTORDBO_H


#include <string>
#include <DBO/Result.h>
#include <DBO/AccessBitsDbo.h>

class SectorDbo
{
public:
    SectorDbo(const std::string& data = "");

    const std::string& getBlock(int block) const;

    std::string getKeyA() const;

    std::string getKeyB() const;

    std::string getAccessBits() const;

    AccessBitsDbo getAccessBitsDbo() const;

    void setBlock(int block, const std::string& data);

    void setKeyA(const std::string& key);

    void setKeyB(const std::string& key);

    void setAccessBits(const std::string& accessBits);

    void setAccessBits(const AccessBitsDbo& accessBits);

    bool hasBlock(int block) const;

    bool hasKeyA() const;

    bool hasKeyB() const;

    bool hasAccessBits() const;

protected:
    std::string _blocks[4];

    bool _haveBlocks[3];

    bool _hasKeyA;

    bool _hasKeyB;

    bool _hasAccessBits;
};


#endif //MIFARE_TOOLS_SECTORDBO_H
