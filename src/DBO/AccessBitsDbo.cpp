//
// Created by robin on 7/21/16.
//

#include <cstring>
#include <iostream>
#include "AccessBitsDbo.h"

const char AccessBitsDbo::nonInvertedBitPosition[4][4] = {
        {0,  0,  0,  0 },
        {11, 10, 9,  8 },
        {23, 22, 21, 20},
        {19, 18, 17, 16}};

const char AccessBitsDbo::invertedBitPosition[4][4] = {
        {0,  0,  0,  0 },
        {7,  6,  5,  4 },
        {3,  2,  1,  0},
        {15, 14, 13, 12}};

AccessBitsDbo::AccessBitsDbo()
    : _bits("\xff\x0f\00\x00", 4)
{
}

AccessBitsDbo::AccessBitsDbo(const std::string &bits)
    : _bits(bits.substr(0, 4))
{
}

const char AccessBitsDbo::getUserData() const
{
    return _bits[_bits.length() - 1];
}

void AccessBitsDbo::setUserData(const char &data)
{
    _bits[_bits.length() - 1] = data;
}

void AccessBitsDbo::setBit(const char& i, const char& j, const bool& value)
{
    char buf[_bits.length()];
    memcpy(buf, _bits.c_str(), _bits.length());

    setArrayBit(buf, nonInvertedBitPosition[i][j], value);
    setArrayBit(buf, invertedBitPosition[i][j], !value);

    _bits = std::string(buf, _bits.length());
}

void AccessBitsDbo::setArrayBit(char *buf, const char &bitPosition, const bool &value)
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

void AccessBitsDbo::setArrayBit(unsigned char *buf, const char &bitPosition, const bool &value)
{
    setArrayBit((char*)buf, bitPosition, value);
}

const bool AccessBitsDbo::getBit(const char &i, const char &j) const
{
    const char* buf = _bits.c_str();
    return getArrayBit(buf, nonInvertedBitPosition[i][j]) && !getArrayBit(buf, invertedBitPosition[i][j]);
}

const bool AccessBitsDbo::getArrayBit(const char *buf, const char &bitPosition)
{
    char byteOffset = (char)(bitPosition / 8);
    char bitOffset = (char)(7 - (bitPosition % 8));
    return (buf[byteOffset] >> bitOffset) & 1 == 1;
}

const bool AccessBitsDbo::getArrayBit(const unsigned char *buf, const char &bitPosition)
{
    return getArrayBit((const char *)buf, bitPosition);
}

const bool AccessBitsDbo::canKeyAReadBlock(const char &block) const
{
    bool c1 = getBit(1, block);
    bool c2 = getBit(2, block);
    bool c3 = getBit(3, block);
    return !c3 || (!c1 && !c2 && c3);
}

const bool AccessBitsDbo::canKeyBReadBlock(const char &block) const
{
    bool c1 = getBit(1, block);
    bool c2 = getBit(2, block);
    bool c3 = getBit(3, block);
    return !c1 || !c2 || !c3;
}

const bool AccessBitsDbo::canKeyAWriteBlock(const char &block) const
{
    bool c1 = getBit(1, block);
    bool c2 = getBit(2, block);
    bool c3 = getBit(3, block);
    return !c1 && !c2 && !c3;
}

const bool AccessBitsDbo::canKeyBWriteBlock(const char &block) const
{
    bool c1 = getBit(1, block);
    bool c2 = getBit(2, block);
    bool c3 = getBit(3, block);
    return (!c2 && !c3) || (c1 && !c3) || (!c1 && c2 && c3);
}

const bool AccessBitsDbo::canKeyAIncrementBlock(const char &block) const
{
    bool c1 = getBit(1, block);
    bool c2 = getBit(2, block);
    bool c3 = getBit(3, block);
    return !c1 && !c2 && !c3;
}

const bool AccessBitsDbo::canKeyBIncrementBlock(const char &block) const
{
    bool c1 = getBit(1, block);
    bool c2 = getBit(2, block);
    bool c3 = getBit(3, block);
    return (!c1 && !c2 && !c3) || (c1 && c2 && !c3);
}

const bool AccessBitsDbo::canKeyADecrementBlock(const char &block) const
{
    bool c1 = getBit(1, block);
    bool c2 = getBit(2, block);
    bool c3 = getBit(3, block);
    return (!c1 && !c2) || (c1 && c2 && !c3);
}

const bool AccessBitsDbo::canKeyBDecrementBlock(const char &block) const
{
    bool c1 = getBit(1, block);
    bool c2 = getBit(2, block);
    bool c3 = getBit(3, block);
    return (!c1 && !c2) || (c1 && c2 && !c3);
}

const bool AccessBitsDbo::canKeyAReadKeyATrailer() const
{
    return false;
}

const bool AccessBitsDbo::canKeyBReadKeyATrailer() const
{
    return false;
}

const bool AccessBitsDbo::canKeyAWriteKeyATrailer() const
{
    bool c1 = getBit(1, 3);
    bool c2 = getBit(2, 3);
    bool c3 = getBit(3, 3);
    return !c1 && !c2;
}

const bool AccessBitsDbo::canKeyBWriteKeyATrailer() const
{
    bool c1 = getBit(1, 3);
    bool c2 = getBit(2, 3);
    bool c3 = getBit(3, 3);
    return (!c1 && c2 && c3) || (c1 && !c2 && !c3);
}

const bool AccessBitsDbo::canKeyAReadAccessBitsTrailer() const
{
    return true;
}

const bool AccessBitsDbo::canKeyBReadAccessBitsTrailer() const
{
    bool c1 = getBit(1, 3);
    bool c2 = getBit(2, 3);
    bool c3 = getBit(3, 3);
    return c1 || (c2 && c3);
}

const bool AccessBitsDbo::canKeyAWriteAccessBitsTrailer() const
{
    bool c1 = getBit(1, 3);
    bool c2 = getBit(2, 3);
    bool c3 = getBit(3, 3);
    return !c1 && !c2 && c3;
}

const bool AccessBitsDbo::canKeyBWriteAccessBitsTrailer() const
{
    bool c1 = getBit(1, 3);
    bool c2 = getBit(2, 3);
    bool c3 = getBit(3, 3);
    return (!c1 && c2 && c3) || (c1 && !c2 && c3);
}

const bool AccessBitsDbo::canKeyAReadKeyBTrailer() const
{
    bool c1 = getBit(1, 3);
    bool c2 = getBit(2, 3);
    bool c3 = getBit(3, 3);
    return (!c1 && !c2) || (!c1 && !c3);
}

const bool AccessBitsDbo::canKeyBReadKeyBTrailer() const
{
    return false;
}

const bool AccessBitsDbo::canKeyAWriteKeyBTrailer() const
{
    bool c1 = getBit(1, 3);
    bool c2 = getBit(2, 3);
    bool c3 = getBit(3, 3);
    return !c1 && !c2;
}

const bool AccessBitsDbo::canKeyBWriteKeyBTrailer() const
{
    bool c1 = getBit(1, 3);
    bool c2 = getBit(2, 3);
    bool c3 = getBit(3, 3);
    return (!c1 && c2 && c3) || (c1 && !c2 && !c3);
}
