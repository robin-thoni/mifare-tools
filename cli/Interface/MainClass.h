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
    enum Actions {
        Read,
        Write,
        Map
    };

    MainClass(int argc, char* argv[]);

    int main();

    std::shared_ptr<NfcDeviceBusiness> getDevice(const std::string& deviceName, std::vector<std::shared_ptr<NfcDeviceBusiness>> devices);

    std::shared_ptr<FreeFareTagBusiness> getTag(const std::string& tagUid, std::vector<std::shared_ptr<FreeFareTagBusiness>> tags);

    int mapKeys(std::shared_ptr<FreeFareTagBusiness> tag, std::vector<std::string> keys);

    int read(std::shared_ptr<FreeFareTagBusiness> tag, std::vector<std::string> keys);

    int write(std::shared_ptr<FreeFareTagBusiness> tag, std::vector<std::string> keys, const std::string& data);

    Result<std::vector<std::string>> readFile(const std::string& filePath);

    Result<std::vector<std::string>> readStream(std::istream& stream);

    void printSectors(const std::vector<SectorDbo>& sectors);

    void printBlockAccessBits(const AccessBitsDbo& accessBits, int block);

    void printTrailerAccessBits(const AccessBitsDbo& accessBits);

    void printVersion();

    std::ostream& cout();

    std::istream& cin();

    static void printPercent(int done, int total, const std::string& header);

    static void printPercentMapKeys(int done, int total);

    static void printPercentDump(int done, int total);

    static void printPercentWrite(int done, int total);

private:
    int _argc;

    char** _argv;

    std::shared_ptr<std::ofstream> _outputStream;

    std::shared_ptr<std::ifstream> _inputStream;

};


#endif //PDNS_SLAVE_MAINCLASS_H
