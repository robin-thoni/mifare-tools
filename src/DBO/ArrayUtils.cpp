//
// Created by robin on 7/23/16.
//

#include "ArrayUtils.h"


void ArrayUtils::setArrayBit(char *buf, const char &bitPosition, const bool &value)
{
    char byteOffset = (char)(bitPosition / 8);
    char bitOffset = (char)(7 - (bitPosition % 8));
    if(value)
    {
        buf[byteOffset] |=  (1 << bitOffset);
    }
    else
    {
        buf[byteOffset] &= ~(1 << bitOffset);
    }
}

void ArrayUtils::setArrayBit(unsigned char *buf, const char &bitPosition, const bool &value)
{
    setArrayBit((char*)buf, bitPosition, value);
}

bool ArrayUtils::getArrayBit(const char *buf, const char &bitPosition)
{
    char byteOffset = (char)(bitPosition / 8);
    char bitOffset = (char)(7 - (bitPosition % 8));
    return (buf[byteOffset] >> bitOffset) & 1 == 1;
}

bool ArrayUtils::getArrayBit(const unsigned char *buf, const char &bitPosition)
{
    return getArrayBit((const char *)buf, bitPosition);
}