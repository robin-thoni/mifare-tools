//
// Created by robin on 8/8/15.
//

#include <iostream>
#include <sysexits.h>
#include <iomanip>
#include <fstream>
#include <DBO/StringUtils.h>
#include <Business/FreeFareDeviceBusiness.h>
#include "DBO/Result.h"
#include "Business/LibNfcBusiness.h"
#include "CommandLineParser.h"
#include "MainClass.h"

#define EX_OUTPUT_ERROR 1
#define EX_INPUT_ERROR 2
#define EX_KEY_ERROR 10
#define EX_LIB_NFC_ERROR 12
#define EX_NFC_DEVICE_NOT_FOUND 13
#define EX_NFC_TAG_NOT_FOUND 14
#define EX_MAP_KEYS_ERROR 15
#define EX_READ_ERROR 16
#define EX_WRITE_ERROR 17

MainClass::MainClass(int argc, char *argv[])
    : _argc(argc)
    , _argv(argv)
{
}

int MainClass::main()
{
    CommandLineParser parser(_argc, _argv);
    CommandLineOption optionVersion(&parser, "version", 'v', "Show libnfc and mifare-tools versions");
    CommandLineOption optionHelp(&parser, "help", 'h', "Show this help");

    CommandLineOption optionMap(&parser, "map", 'm', "Map keys for the tag");
    CommandLineOption optionRead(&parser, "read", 'r', "Read the tag");
    CommandLineOption optionWrite(&parser, "write", 'w', "Write the tag");
    CommandLineOption optionDevices(&parser, "devices", 'd', "List NFC devices");
    CommandLineOption optionTags(&parser, "tags", 't', "List NFC tags");

    CommandLineOption optionDevice(&parser, "device", 'e', "Use the device DEVICE", "DEVICE");
    CommandLineOption optionTag(&parser, "tag", 'u', "Use the UID tag", "UID");

    CommandLineOption optionKeyFile(&parser, "key-file", 'f', "Path to a file containing keys", "FILE");
    CommandLineOption optionKey(&parser, "key", 'k', "Key to use to authenticate", "KEY");

    CommandLineOption optionOutput(&parser, "output", 'o', "Redirect output to FILE. '-' to use stdout", "FILE", "-");
    CommandLineOption optionInput(&parser, "input", 'i', "Read input from FILE. '-' to use stdin", "FILE", "-");

    if (!parser.parse()) {
        return parser.showHelp(EX_USAGE);
    }
    std::string outputFile = optionOutput.getDefaultValue();
    if (optionOutput.isSet()) {
        outputFile = optionOutput.getValue();
    }

    if (outputFile != "-" && !outputFile.empty()) {
        std::shared_ptr<std::ofstream> fileCout = std::make_shared<std::ofstream>();
        fileCout->open(outputFile);
        if (!*fileCout) {
            std::cerr << "Failed to redirect output: " << strerror(errno) << std::endl;
            return EX_OUTPUT_ERROR;
        }
        _outputStream = fileCout;
    }

    std::string inputFile = optionInput.getDefaultValue();
    if (optionInput.isSet()) {
        inputFile = optionInput.getValue();
    }
    if (inputFile != "-" && !inputFile.empty()) {
        std::shared_ptr<std::ifstream> fileCin = std::make_shared<std::ifstream>();
        fileCin->open(inputFile);
        if (!*fileCin) {
            std::cerr << "Failed to open input file: " << strerror(errno) << std::endl;
            return EX_INPUT_ERROR;
        }
        _inputStream = fileCin;
    }

    if (optionVersion.isSet()) {
        printVersion();
        return EX_OK;
    }
    if (optionHelp.isSet()) {
        return parser.showHelp(EX_OK, false);
    }

    std::string deviceName = optionDevice.getDefaultValue();
    if (optionDevice.isSet()) {
        deviceName = optionDevice.getValue();
    }

    std::string tagUid = optionTag.getDefaultValue();
    if (optionTag.isSet()) {
        tagUid = optionTag.getValue();
    }

    std::vector<std::string> keys;
    if (optionKeyFile.isSet()) {
        for (auto filePath : optionKeyFile.getValues()) {
            auto keysResult = readFile(filePath);
            if (!keysResult) {
                keysResult.print();
                return EX_KEY_ERROR;
            }
            auto fileKeys = keysResult.getData();
            keys.insert(keys.end(), fileKeys.begin(), fileKeys.end());
        }
    }
    if (optionKey.isSet()) {
        for (auto key : optionKey.getValues()) {
            auto keyResult = StringUtils::humanToRaw(key);
            key = keyResult.getData();
            if (!keyResult || key.length() != 6) {
                std::cerr << "Invalid key" << std::endl;
                return EX_KEY_ERROR;
            }
            keys.push_back(key);
        }
    }

    std::string inputData = "";

    int res = EX_OK;
    Actions action;
    if (optionRead.isSet()) {
        action = Read;
    }
    else if (optionMap.isSet()) {
        action = Map;
    }
    else if (optionWrite.isSet()) {
        auto readResult = readStream(cin());
        if (!readResult) {
            readResult.print();
            return EX_INPUT_ERROR;
        }
        for (auto data : readResult.getData()) {
            inputData += data;
        }
        action = Write;
    }
    else {
        std::cerr << "Must select an action (map|read|write|devices|tags)" << std::endl;
        return EX_USAGE;
    }

    LibNfcBusiness libNfc;
    auto init = libNfc.init();
    if (!init) {
        init.print();
        res = EX_LIB_NFC_ERROR;
    }
    else
    {
        auto devicesResult = libNfc.getDevices();
        if (!devicesResult)
        {
            devicesResult.print();
            res = EX_LIB_NFC_ERROR;
        }
        else
        {
            auto devices = devicesResult.getData();

            if (optionDevices.isSet())
            {
                for (auto device : devices) {
                    cout() << device->getConnStr() << std::endl;
                }
            }
            else
            {
                auto device = getDevice(deviceName, devices);
                if (device == 0)
                {
                    std::cerr << "NFC device not found" << std::endl;
                    res = EX_NFC_DEVICE_NOT_FOUND;
                }
                else {
                    auto open = device->open();
                    if (!open) {
                        open.print();
                        res = EX_LIB_NFC_ERROR;
                    }
                    else {

                        FreeFareDeviceBusiness freeFareDevice(device);
                        auto tagsResult = freeFareDevice.getTags();
                        if (!tagsResult)
                        {
                            tagsResult.print();
                            res = EX_LIB_NFC_ERROR;
                        }

                        auto tags = tagsResult.getData();

                        if (optionTags.isSet()) {
                            for (auto tag : tags)
                            {
                                cout() << "UID=" << tag->getUid() << " \tType=" << tag->getType() << std::endl;
                            }
                        }
                        else {
                            auto tag = getTag(tagUid, tags);
                            if (tag == 0) {
                                std::cerr << "Tag not found" << std::endl;
                                res = EX_NFC_TAG_NOT_FOUND;
                            }
                            else {
                                if (action == Read) {
                                    res = read(tag, keys);
                                }
                                else if (action == Map) {
                                    res = mapKeys(tag, keys);
                                }
                                else if (action == Write) {
                                    res = write(tag, keys, inputData);
                                }
                            }
                        }
                    }
                    device->close();
                }
            }
        }
        libNfc.clean();
    }

    if (_outputStream != 0) {
        _outputStream->close();
    }
    if (_inputStream != 0) {
        _inputStream->close();
    }
    return res;
}

int MainClass::mapKeys(std::shared_ptr<FreeFareTagBusiness> tag, std::vector<std::string> keys)
{
    auto mappedKeysResult = tag->mapKeys(keys, printPercentMapKeys);
    if (!mappedKeysResult) {
        mappedKeysResult.print();
        return EX_MAP_KEYS_ERROR;
    }
    else {
        auto mappedKeys = mappedKeysResult.getData();
        for (int s = 0; s < mappedKeys.size(); ++s) {
            auto sectorKey = mappedKeys[s];
            cout() << "+Sector: " << s << std::endl;
            for (int b = 0; b < 4; ++b) {
                cout() << "+Block: " << b << std::endl;
                cout() << "+Key: A" << std::endl << (!sectorKey.first.empty() ? StringUtils::rawToHuman(sectorKey.first) : std::string(12, '-')) << std::endl;
                cout() << "+Key: B" << std::endl << (!sectorKey.second.empty() ? StringUtils::rawToHuman(sectorKey.second) : std::string(12, '-')) << std::endl;
            }
        }
    }
    return EX_OK;
}

int MainClass::read(std::shared_ptr<FreeFareTagBusiness> tag, std::vector<std::string> keys)
{
    auto readResult = tag->read(keys, printPercentMapKeys, printPercentDump);
    if (!readResult) {
        readResult.print();
        return EX_READ_ERROR;
    }
    auto read = readResult.getData();
    printSectors(read);
    return EX_OK;
}

int MainClass::write(std::shared_ptr<FreeFareTagBusiness> tag, std::vector<std::string> keys, const std::string &data)
{
    auto writeResult = tag->write(keys, data, false, printPercentMapKeys, printPercentWrite);
    if (!writeResult) {
        writeResult.print();
        return EX_WRITE_ERROR;
    }
//    std::vector<SectorDbo> sectors;
//    std::string d = StringUtils::ensureSize(data, 1024);
//    for (int i = 0; i < 16; ++i) {
//        SectorDbo sectorDbo(d.substr(i * 64, 64));
//        sectors.push_back(sectorDbo);
//    }
//    printSectors(sectors);
    return EX_OK;
}

void MainClass::printSectors(const std::vector<SectorDbo> &sectors)
{
    for(int s = 0; s < sectors.size(); ++s) {
        cout() << "+Sector: " << s << std::endl;
        auto sector = sectors[s];
        for (int b = 0; b < 3; ++b) {
            cout() << (sector.hasBlock(b) ? StringUtils::rawToHuman(sector.getBlock(b)) : std::string(32, '-')) << std::endl;
        }
        cout() << (sector.hasKeyA() ? StringUtils::rawToHuman(sector.getKeyA()) : std::string(12, '-'))
        << (sector.hasAccessBits() ? StringUtils::rawToHuman(sector.getAccessBits()) : std::string(8, '-'))
        << (sector.hasKeyB() ? StringUtils::rawToHuman(sector.getKeyB()) : std::string(12, '-')) << std::endl;


        cout() << "+Trailer key A: " << (sector.hasKeyA() ? StringUtils::rawToHuman(sector.getKeyA()) : std::string(12, '-'))
        << "\t AC bits: " << (sector.hasAccessBits() ? StringUtils::rawToHuman(sector.getAccessBits()) : std::string(8, '-'))
        << "\t key B: " << (sector.hasKeyB() ? StringUtils::rawToHuman(sector.getKeyB()) : std::string(12, '-')) << std::endl;
        AccessBitsDbo accessBitsDbo = sector.getAccessBitsDbo();
        for (int b = 0; b < 3; ++b) {
            cout() << "+Block: " << b << " ";
            printBlockAccessBits(accessBitsDbo, b);
        }
        cout() << "+Block: 3 ";
        printTrailerAccessBits(accessBitsDbo);
    }
}

void MainClass::printBlockAccessBits(const AccessBitsDbo &accessBits, int block)
{
    cout() << "read: " << (accessBits.canKeyAReadBlock(block) ? "A" : " ") << (accessBits.canKeyBReadBlock(block) ? "B" : " ");
    cout() << "\t write: " << (accessBits.canKeyAWriteBlock(block) ? "A" : " ") << (accessBits.canKeyBWriteBlock(block) ? "B" : " ");
    cout() << "\t increment: " << (accessBits.canKeyAIncrementBlock(block) ? "A" : " ") << (accessBits.canKeyBIncrementBlock(block) ? "B" : " ");
    cout() << "\t decrement: " << (accessBits.canKeyADecrementBlock(block) ? "A" : " ") << (accessBits.canKeyBDecrementBlock(block) ? "B" : " ") << std::endl;
}

void MainClass::printTrailerAccessBits(const AccessBitsDbo &accessBits)
{
    cout() << "key A read: " << (accessBits.canKeyAReadKeyATrailer() ? "A" : " ") << (accessBits.canKeyBReadKeyATrailer() ? "B" : " ");
    cout() << "\t key A write: " << (accessBits.canKeyAWriteKeyATrailer() ? "A" : " ") << (accessBits.canKeyBWriteKeyATrailer() ? "B" : " ");

    cout() << "\t AC bits read: " << (accessBits.canKeyAReadAccessBitsTrailer() ? "A" : " ") << (accessBits.canKeyBReadAccessBitsTrailer() ? "B" : " ");
    cout() << "\t AC bits write: " << (accessBits.canKeyAWriteAccessBitsTrailer() ? "A" : " ") << (accessBits.canKeyBWriteAccessBitsTrailer() ? "B" : " ");

    cout() << "\t key B read: " << (accessBits.canKeyAReadKeyBTrailer() ? "A" : " ") << (accessBits.canKeyBReadKeyBTrailer() ? "B" : " ");
    cout() << "\t key B write: " << (accessBits.canKeyAWriteKeyBTrailer() ? "A" : " ") << (accessBits.canKeyBWriteKeyBTrailer() ? "B" : " ") << std::endl;;
}

void MainClass::printPercent(int done, int total, const std::string& header)
{
    if (isatty(fileno(stdout))) {
        std::cout << "\r\033[2K" << header << ": " << std::fixed << std::setprecision(1)
        << ((float) done / (float) total * 100.0) << "%" << std::flush;
        if (done == total) {
            std::cout << std::endl;
        }
    }
}

void MainClass::printPercentMapKeys(int done, int total)
{
    printPercent(done, total, "Mapping keys");
}

void MainClass::printPercentDump(int done, int total)
{
    printPercent(done, total, "Dumping");
}

void MainClass::printPercentWrite(int done, int total)
{
    printPercent(done, total, "Writing");
}

void MainClass::printVersion()
{
    cout() << "LibNfc version: " << LibNfcBusiness::getLibNfcVersion() << std::endl;
    cout() << "Mifare-tools version: " << LibNfcBusiness::getMifareToolsVersion() << std::endl;
}

std::shared_ptr<NfcDeviceBusiness> MainClass::getDevice(const std::string &deviceName, std::vector<std::shared_ptr<NfcDeviceBusiness>> devices)
{
    if (deviceName.empty()) {
        if (devices.size() > 0) {
            return devices[0];
        }
    }
    else {
        for (auto d : devices) {
            if (d->getConnStr() == deviceName) {
                return d;
            }
        }
    }
    return 0;
}

std::shared_ptr<FreeFareTagBusiness> MainClass::getTag(const std::string &tagUid,
                                                       std::vector<std::shared_ptr<FreeFareTagBusiness>> tags)
{
    if (tagUid.empty()) {
        if (tags.size() > 0) {
            return tags[0];
        }
    }
    else {
        for (auto t : tags) {
            if (t->getUid() == tagUid) {
                return t;
            }
        }
    }
    return 0;
}

Result<std::vector<std::string>> MainClass::readFile(const std::string &filePath)
{
    std::ifstream fileInput(filePath);
    if (fileInput) {
        return readStream(fileInput);
    }
    else {
        return Result<std::vector<std::string>>::error("Failed to open file: " + std::string(strerror(errno)));
    }
}

Result<std::vector<std::string>> MainClass::readStream(std::istream &stream)
{
    std::vector<std::string> lines;
    while (!stream.eof()) {
        std::string line;
        std::getline(stream, line);
        line = StringUtils::removeSpaces(line);
        if (line.compare(0, 1, "#") != 0 && line.compare(0, 1, "+") != 0) {
            std::replace(line.begin(), line.end(), '-', '0');
            auto keyResult = StringUtils::humanToRaw(line);
            if (!keyResult) {
                return Result<std::vector<std::string>>::error("Invalid data");
            }
            line = keyResult.getData();
            lines.push_back(line);
        }
    }
    return Result<std::vector<std::string>>::ok(lines);
}

std::ostream &MainClass::cout()
{
    return _outputStream == 0 ? std::cout : *_outputStream;
}

std::istream &MainClass::cin()
{
    return _inputStream == 0 ? std::cin : *_inputStream;
}
