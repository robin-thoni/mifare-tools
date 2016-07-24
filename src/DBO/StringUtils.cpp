//
// Created by robin on 6/20/16.
//

#include <algorithm>
#include "StringUtils.h"

const std::string StringUtils::hex = "0123456789abcdef";

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

std::string StringUtils::rawToHuman(const std::string& raw)
{
    std::string human;
    for (size_t i = 0; i < raw.size(); ++i) {
        human += rawToHuman(raw[i]);
    }
    return human;
}

ResultString StringUtils::humanToRaw(const std::string& human)
{
    if (human.size() % 2 != 0) {
        return ResultString::error("Malformed hex data: invalid length");
    }
    std::string raw;
    for (size_t i = 0; i < human.size(); i += 2) {
        char c1 = toLower(human[i]);
        char c2 = toLower(human[i + 1]);
        size_t i1 = hex.find(c1);
        size_t i2 = hex.find(c2);
        if (i1 == std::string::npos || i2 == std::string::npos) {
            return ResultString::error("Malformed hex data at char " + std::to_string(i));
        }
        raw += std::string(1, (char)((i1 << 4) + i2));
    }
    return ResultString::ok(raw);
}

std::string StringUtils::toLower(const std::string& str)
{
    std::string lower;
    for (size_t i = 0; i < str.size(); ++i) {
        lower += tolower(str[i]);
    }
    return lower;
}

char StringUtils::toLower(char c)
{
    if (c >= 'A' && c <= 'Z') {
        return c + ('a' - 'A');
    }
    return c;
}

std::string StringUtils::toUpper(const std::string& str)
{
    std::string upper;
    for (size_t i = 0; i < str.size(); ++i) {
        upper += toUpper(str[i]);
    }
    return upper;
}

char StringUtils::toUpper(char c)
{
    if (c >= 'a' && c <= 'z') {
        return c - ('a' - 'A');
    }
    return c;
}

std::string StringUtils::ensureSize(const std::string &data, int size)
{
    if (data.size() == size) {
        return data;
    }
    else if (data.size() > size) {
        return data.substr(0, size);
    }
    else {
        return data + std::string(size - data.size(), '\0');
    }
}

std::string StringUtils::ltrim(const std::string& s)
{
    std::string data = s;
    data.erase(data.begin(), std::find_if(data.begin(), data.end(),
                                    std::not1(std::ptr_fun<int, int>(std::isspace))));
    return data;
}

std::string StringUtils::rtrim(const std::string& s)
{
    std::string data = s;
    data.erase(std::find_if(data.rbegin(), data.rend(),
                         std::not1(std::ptr_fun<int, int>(std::isspace))).base(), data.end());
    return data;
}

std::string StringUtils::trim(const std::string& s)
{
    return ltrim(rtrim(s));
}

std::string StringUtils::removeSpaces(const std::string &s)
{
    std::string data = s;
    data.erase(std::remove_if(data.begin(), data.end(), std::ptr_fun<int, int>(std::isspace)), data.end());
    return data;
}
