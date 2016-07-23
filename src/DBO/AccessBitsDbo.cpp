//
// Created by robin on 7/21/16.
//

#include <cstring>
#include <iostream>
#include "AccessBitsDbo.h"
#include "StringUtils.h"
#include "ArrayUtils.h"

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
    : _bits(StringUtils::ensureSize(bits, 4))
{
}

char AccessBitsDbo::getUserData() const
{
    return _bits[_bits.length() - 1];
}

void AccessBitsDbo::setUserData(const char &data)
{
    _bits[_bits.length() - 1] = data;
}

void AccessBitsDbo::setBit(int i, int j, const bool& value)
{
    char buf[_bits.length()];
    memcpy(buf, _bits.c_str(), _bits.length());

    ArrayUtils::setArrayBit(buf, nonInvertedBitPosition[i][j], value);
    ArrayUtils::setArrayBit(buf, invertedBitPosition[i][j], !value);

    _bits = std::string(buf, _bits.length());
}

bool AccessBitsDbo::getBit(int i, int j) const
{
    const char* buf = _bits.c_str();
    return ArrayUtils::getArrayBit(buf, nonInvertedBitPosition[i][j]) && !ArrayUtils::getArrayBit(buf, invertedBitPosition[i][j]);
}

std::string AccessBitsDbo::getBits() const
{
    return _bits;
}

bool AccessBitsDbo::canKeyAReadBlock(int block) const
{
    bool c1 = getBit(1, block);
    bool c2 = getBit(2, block);
    bool c3 = getBit(3, block);
    return !c3 || (!c1 && !c2 && c3);
}

bool AccessBitsDbo::canKeyBReadBlock(int block) const
{
    bool c1 = getBit(1, block);
    bool c2 = getBit(2, block);
    bool c3 = getBit(3, block);
    return !c1 || !c2 || !c3;
}

bool AccessBitsDbo::canKeyAWriteBlock(int block) const
{
    bool c1 = getBit(1, block);
    bool c2 = getBit(2, block);
    bool c3 = getBit(3, block);
    return !c1 && !c2 && !c3;
}

bool AccessBitsDbo::canKeyBWriteBlock(int block) const
{
    bool c1 = getBit(1, block);
    bool c2 = getBit(2, block);
    bool c3 = getBit(3, block);
    return (!c2 && !c3) || (c1 && !c3) || (!c1 && c2 && c3);
}

bool AccessBitsDbo::canKeyAIncrementBlock(int block) const
{
    bool c1 = getBit(1, block);
    bool c2 = getBit(2, block);
    bool c3 = getBit(3, block);
    return !c1 && !c2 && !c3;
}

bool AccessBitsDbo::canKeyBIncrementBlock(int block) const
{
    bool c1 = getBit(1, block);
    bool c2 = getBit(2, block);
    bool c3 = getBit(3, block);
    return (!c1 && !c2 && !c3) || (c1 && c2 && !c3);
}

bool AccessBitsDbo::canKeyADecrementBlock(int block) const
{
    bool c1 = getBit(1, block);
    bool c2 = getBit(2, block);
    bool c3 = getBit(3, block);
    return (!c1 && !c2) || (c1 && c2 && !c3);
}

bool AccessBitsDbo::canKeyBDecrementBlock(int block) const
{
    bool c1 = getBit(1, block);
    bool c2 = getBit(2, block);
    bool c3 = getBit(3, block);
    return (!c1 && !c2) || (c1 && c2 && !c3);
}

bool AccessBitsDbo::canKeyAReadKeyATrailer() const
{
    return false;
}

bool AccessBitsDbo::canKeyBReadKeyATrailer() const
{
    return false;
}

bool AccessBitsDbo::canKeyAWriteKeyATrailer() const
{
    bool c1 = getBit(1, 3);
    bool c2 = getBit(2, 3);
    bool c3 = getBit(3, 3);
    return !c1 && !c2;
}

bool AccessBitsDbo::canKeyBWriteKeyATrailer() const
{
    bool c1 = getBit(1, 3);
    bool c2 = getBit(2, 3);
    bool c3 = getBit(3, 3);
    return (!c1 && c2 && c3) || (c1 && !c2 && !c3);
}

bool AccessBitsDbo::canKeyAReadAccessBitsTrailer() const
{
    return true;
}

bool AccessBitsDbo::canKeyBReadAccessBitsTrailer() const
{
    bool c1 = getBit(1, 3);
    bool c2 = getBit(2, 3);
    bool c3 = getBit(3, 3);
    return c1 || (c2 && c3);
}

bool AccessBitsDbo::canKeyAWriteAccessBitsTrailer() const
{
    bool c1 = getBit(1, 3);
    bool c2 = getBit(2, 3);
    bool c3 = getBit(3, 3);
    return !c1 && !c2 && c3;
}

bool AccessBitsDbo::canKeyBWriteAccessBitsTrailer() const
{
    bool c1 = getBit(1, 3);
    bool c2 = getBit(2, 3);
    bool c3 = getBit(3, 3);
    return (!c1 && c2 && c3) || (c1 && !c2 && c3);
}

bool AccessBitsDbo::canKeyAReadKeyBTrailer() const
{
    bool c1 = getBit(1, 3);
    bool c2 = getBit(2, 3);
    bool c3 = getBit(3, 3);
    return (!c1 && !c2) || (!c1 && !c3);
}

bool AccessBitsDbo::canKeyBReadKeyBTrailer() const
{
    return false;
}

bool AccessBitsDbo::canKeyAWriteKeyBTrailer() const
{
    bool c1 = getBit(1, 3);
    bool c2 = getBit(2, 3);
    bool c3 = getBit(3, 3);
    return !c1 && !c2;
}

bool AccessBitsDbo::canKeyBWriteKeyBTrailer() const
{
    bool c1 = getBit(1, 3);
    bool c2 = getBit(2, 3);
    bool c3 = getBit(3, 3);
    return (!c1 && c2 && c3) || (c1 && !c2 && !c3);
}
