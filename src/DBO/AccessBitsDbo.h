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

    void setBit(int i, int j, const bool& value);
    bool getBit(int i, int j) const;
    std::string getBits() const;

    bool canKeyAReadBlock(int block) const;
    bool canKeyBReadBlock(int block) const;

    bool canKeyAWriteBlock(int block) const;
    bool canKeyBWriteBlock(int block) const;

    bool canKeyAIncrementBlock(int block) const;
    bool canKeyBIncrementBlock(int block) const;

    bool canKeyADecrementBlock(int block) const;
    bool canKeyBDecrementBlock(int block) const;

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

private:
    std::string _bits;
};


#endif //MIFARE_TOOLS_ACCESSBITSDBO_H
