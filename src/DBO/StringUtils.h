//
// Created by robin on 6/20/16.
//

#ifndef MIFARE_TOOLS_HEX_H
#define MIFARE_TOOLS_HEX_H


#include <string>
#include <DBO/Result.h>

class StringUtils
{
public:
    static const std::string hex;

    static std::string rawToHuman(unsigned char c);
    static std::string rawToHuman(char c);
    static std::string rawToHuman(unsigned short c);
    static std::string rawToHuman(short c);
    static std::string rawToHuman(unsigned int c);
    static std::string rawToHuman(int c);
    static std::string rawToHuman(unsigned long c);
    static std::string rawToHuman(long c);
    static std::string rawToHuman(const std::string& raw);

    static ResultString humanToRaw(const std::string& human);

    static std::string toLower(const std::string& str);
    static char toLower(char c);

    static std::string toUpper(const std::string& str);
    static char toUpper(char c);

    static std::string ensureSize(const std::string& data, int size);

    static std::string ltrim(const std::string& s);
    static std::string rtrim(const std::string& s);
    static std::string trim(const std::string& s);
    static std::string removeSpaces(const std::string& s);
};


#endif //MIFARE_TOOLS_HEX_H
