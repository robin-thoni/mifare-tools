//
// Created by robin on 7/22/16.
//

#ifndef MIFARE_TOOLS_SECTORDBO_H
#define MIFARE_TOOLS_SECTORDBO_H


#include <string>
#include <DBO/Result.h>

class SectorDbo
{
public:

    const std::string& getBlock(int block) const;

    std::string getKeyA() const;

    std::string getKeyB() const;

    std::string getAccessBits() const;

    static Result<SectorDbo> parse(std::string data);

protected:
    std::string _blocks[4];
};


#endif //MIFARE_TOOLS_SECTORDBO_H
