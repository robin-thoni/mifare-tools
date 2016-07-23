//
// Created by robin on 8/8/15.
//

#ifndef PDNS_SLAVE_MAINCLASS_H
#define PDNS_SLAVE_MAINCLASS_H

#include <memory>
#include <Business/FreeFareTagBusiness.h>
#include <Business/NfcDeviceBusiness.h>

class MainClass {
public:
    MainClass(int argc, char* argv[]);

    int main();

    std::shared_ptr<NfcDeviceBusiness> getDevice(const std::string& deviceName, std::vector<std::shared_ptr<NfcDeviceBusiness>> devices);

    std::shared_ptr<FreeFareTagBusiness> getTag(const std::string& tagUid, std::vector<std::shared_ptr<FreeFareTagBusiness>> tags);

    int mapKeys(std::shared_ptr<FreeFareTagBusiness> tag, std::vector<std::string> keys);

    int dump(std::shared_ptr<FreeFareTagBusiness> tag, std::vector<std::string> keys);

    Result<std::vector<std::string>> readFile(const std::string& filePath);

    void printBlockAccessBits(const AccessBitsDbo& accessBits, int block);

    void printTrailerAccessBits(const AccessBitsDbo& accessBits);

    void printVersion() const;

    static void printPercent(int done, int total, const std::string& header);

    static void printPercentMapKeys(int done, int total);

    static void printPercentDump(int done, int total);

private:
    int _argc;

    char** _argv;

};


#endif //PDNS_SLAVE_MAINCLASS_H
