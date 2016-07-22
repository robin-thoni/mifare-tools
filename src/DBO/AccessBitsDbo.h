//
// Created by robin on 7/21/16.
//

#ifndef MIFARE_TOOLS_ACCESSBITSDBO_H
#define MIFARE_TOOLS_ACCESSBITSDBO_H


#include <string>

#ifndef FRIEND_TEST
#define FRIEND_TEST(C, T)
#endif

class AccessBitsDbo
{
public:

    static const char nonInvertedBitPosition[4][4];
    static const char invertedBitPosition[4][4];

    AccessBitsDbo();
    AccessBitsDbo(const std::string& bits);

    const char getUserData() const;
    void setUserData(const char& data);

    void setBit(const char& i, const char& j, const bool& value);
    const bool getBit(const char& i, const char& j) const;

    const bool canKeyAReadBlock(const char& block) const;
    const bool canKeyBReadBlock(const char& block) const;

    const bool canKeyAWriteBlock(const char& block) const;
    const bool canKeyBWriteBlock(const char& block) const;

    const bool canKeyAIncrementBlock(const char& block) const;
    const bool canKeyBIncrementBlock(const char& block) const;

    const bool canKeyADecrementBlock(const char& block) const;
    const bool canKeyBDecrementBlock(const char& block) const;

    const bool canKeyAReadKeyATrailer() const;
    const bool canKeyBReadKeyATrailer() const;

    const bool canKeyAWriteKeyATrailer() const;
    const bool canKeyBWriteKeyATrailer() const;

    const bool canKeyAReadAccessBitsTrailer() const;
    const bool canKeyBReadAccessBitsTrailer() const;

    const bool canKeyAWriteAccessBitsTrailer() const;
    const bool canKeyBWriteAccessBitsTrailer() const;

    const bool canKeyAReadKeyBTrailer() const;
    const bool canKeyBReadKeyBTrailer() const;

    const bool canKeyAWriteKeyBTrailer() const;
    const bool canKeyBWriteKeyBTrailer() const;

    static void setArrayBit(char *buf, const char &bitPosition, const bool &value);
    static void setArrayBit(unsigned char *buf, const char &bitPosition, const bool &value);
    static const bool getArrayBit(const char *buf, const char &bitPosition);
    static const bool getArrayBit(const unsigned char *buf, const char &bitPosition);

private:
    std::string _bits;
};


#endif //MIFARE_TOOLS_ACCESSBITSDBO_H
