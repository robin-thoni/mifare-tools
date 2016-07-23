//
// Created by robin on 7/23/16.
//

#ifndef MIFARE_TOOLS_ARRAYUTILS_H
#define MIFARE_TOOLS_ARRAYUTILS_H


class ArrayUtils
{
public:
    static void setArrayBit(char *buf, const char &bitPosition, const bool &value);
    static void setArrayBit(unsigned char *buf, const char &bitPosition, const bool &value);
    static bool getArrayBit(const char *buf, const char &bitPosition);
    static bool getArrayBit(const unsigned char *buf, const char &bitPosition);

};


#endif //MIFARE_TOOLS_ARRAYUTILS_H
