//
// Created by robin on 6/19/16.
//

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

const freefare_tag *FreeFareTag::getTag() const
{
    return _tag;
}

freefare_tag_type FreeFareTag::getType() const
{
    return _type;
}

const std::string &FreeFareTag::getUid() const
{
    return _uid;
}