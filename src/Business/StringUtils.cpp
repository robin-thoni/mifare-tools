//
// Created by robin on 6/20/16.
//

#include "StringUtils.h"

const char* StringUtils::hex = "0123456789abcdef";

std::string StringUtils::rawToHuman(unsigned char c)
{
    return std::string(1, hex[c >> 4]) + std::string(1, hex[c & 0x0f]);
}

std::string StringUtils::rawToHuman(char c)
{
    return rawToHuman((unsigned char)c);
}

std::string StringUtils::rawToHuman(unsigned short c)
{
    return rawToHuman((unsigned char)(c >> 8)) + rawToHuman((unsigned char)c);
}

std::string StringUtils::rawToHuman(short c)
{
    return rawToHuman((unsigned short)c);
}

std::string StringUtils::rawToHuman(unsigned int c)
{
    return  rawToHuman((unsigned short)(c >> 16)) + rawToHuman((unsigned short)c);
}

std::string StringUtils::rawToHuman(int c)
{
    return rawToHuman((unsigned int)c);
}

std::string StringUtils::rawToHuman(unsigned long c)
{
    return  rawToHuman((unsigned int)(c >> 32)) + rawToHuman((unsigned int)c);
}

std::string StringUtils::rawToHuman(long c)
{
    return rawToHuman((unsigned long)c);
}

std::string StringUtils::rawToHuman(std::string raw)
{
    std::string human;
    for (size_t i = 0; i < raw.size(); ++i) {
        human += rawToHuman(raw[i]);
    }
    return human;
}

ResultString StringUtils::humanToRaw(std::string human)
{
    std::string raw;
    return ResultString::ok(raw);
}
