//
// Created by robin on 6/19/16.
//

#ifndef MIFARE_TOOLS_FREEFARETAG_H
#define MIFARE_TOOLS_FREEFARETAG_H

#include <freefare.h>
#include <string>

class FreeFareTag
{
public:
    FreeFareTag(FreefareTag tag);
    ~FreeFareTag();

    const freefare_tag * getTag() const;

    freefare_tag_type getType() const;

    const std::string & getUid() const;

private:
    FreefareTag _tag;

    freefare_tag_type _type;

    std::string _uid;
};


#endif //MIFARE_TOOLS_FREEFARETAG_H
