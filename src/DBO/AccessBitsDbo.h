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

    char getUserData() const;
    void setUserData(const char& data);

    void setBit(const char& i, const char& j, const bool& value);
    bool getBit(const char& i, const char& j) const;

    bool canKeyAReadBlock(const char& block) const;
    bool canKeyBReadBlock(const char& block) const;

    bool canKeyAWriteBlock(const char& block) const;
    bool canKeyBWriteBlock(const char& block) const;

    bool canKeyAIncrementBlock(const char& block) const;
    bool canKeyBIncrementBlock(const char& block) const;

    bool canKeyADecrementBlock(const char& block) const;
    bool canKeyBDecrementBlock(const char& block) const;

    bool canKeyAReadKeyATrailer() const;
    bool canKeyBReadKeyATrailer() const;

    bool canKeyAWriteKeyATrailer() const;
    bool canKeyBWriteKeyATrailer() const;

    bool canKeyAReadAccessBitsTrailer() const;
    bool canKeyBReadAccessBitsTrailer() const;

    bool canKeyAWriteAccessBitsTrailer() const;
    bool canKeyBWriteAccessBitsTrailer() const;

    bool canKeyAReadKeyBTrailer() const;
    bool canKeyBReadKeyBTrailer() const;

    bool canKeyAWriteKeyBTrailer() const;
    bool canKeyBWriteKeyBTrailer() const;

    static void setArrayBit(char *buf, const char &bitPosition, const bool &value);
    static void setArrayBit(unsigned char *buf, const char &bitPosition, const bool &value);
    static bool getArrayBit(const char *buf, const char &bitPosition);
    static bool getArrayBit(const unsigned char *buf, const char &bitPosition);

private:
    std::string _bits;
};


#endif //MIFARE_TOOLS_ACCESSBITSDBO_H
