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

#define EX_REDIRECT_ERROR 1
#define EX_KEY_ERROR 10
#define EX_LIB_NFC_ERROR 12
#define EX_NFC_DEVICE_NOT_FOUND 13
#define EX_NFC_TAG_NOT_FOUND 14
#define EX_MAP_KEYS_ERROR 15
#define EX_DUMP_ERROR 16

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
    CommandLineOption optionDump(&parser, "dump", 'd', "Dump the tag");
    CommandLineOption optionDevices(&parser, "devices", 'l', "List NFC devices");
    CommandLineOption optionTags(&parser, "tags", 't', "List NFC tags");

    CommandLineOption optionDevice(&parser, "device", 'e', "Use the device DEVICE", "DEVICE");
    CommandLineOption optionTag(&parser, "tag", 'u', "Use the UID tag", "UID");

    CommandLineOption optionKeyFile(&parser, "key-file", 'f', "Path to a file containing keys", "FILE");
    CommandLineOption optionKey(&parser, "key", 'k', "Key to use to authenticate", "KEY");

    CommandLineOption optionOutput(&parser, "output", 'o', "Redirect output to FILE. '-' to use stdout", "FILE", "-");

    if (!parser.parse()) {
        return parser.showHelp(EX_USAGE);
    }
    std::string outputFile = optionOutput.getDefaultValue();
    if (optionOutput.isSet()) {
        outputFile = optionOutput.getValue();
    }

    std::shared_ptr<std::ofstream> fileCout = 0;
    if (outputFile != "-" && !outputFile.empty()) {
        fileCout = std::make_shared<std::ofstream>();
        fileCout->open(outputFile);
        if (!*fileCout) {
            std::cerr << "Failed to redirect output: " << strerror(errno) << std::endl;
            return EX_REDIRECT_ERROR;
        }
        _outputStream = fileCout;
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

    int res = EX_OK;
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
                                if (optionDump.isSet()) {
                                    res = dump(tag, keys);
                                }
                                else if (optionMap.isSet()) {
                                    res = mapKeys(tag, keys);
                                }
                                else {
                                    std::cerr << "Must select an action (map|dump|devices|tags)" << std::endl;
                                    res = EX_USAGE;
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

int MainClass::dump(std::shared_ptr<FreeFareTagBusiness> tag, std::vector<std::string> keys)
{
    auto dumpResult = tag->dump(keys, printPercentMapKeys, printPercentDump);
    if (!dumpResult) {
        dumpResult.print();
        return EX_DUMP_ERROR;
    }
    auto dump = dumpResult.getData();
    for(int s = 0; s < 16; ++s) {
        cout() << "+Sector: " << s << std::endl;
        auto sector = dump[s];
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
    return EX_OK;
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
    std::vector<std::string> lines;
    std::ifstream fileInput(filePath);
    if (fileInput) {
        while (!fileInput.eof()) {
            std::string line;
            std::getline(fileInput, line);
            line = StringUtils::removeSpaces(line);
            if (line.compare(0, 1, "#") != 0 && line.compare(0, 1, "+") != 0) {
                auto keyResult = StringUtils::humanToRaw(line);
                if (!keyResult) {
                    return Result<std::vector<std::string>>::error("Invalid file data");
                }
                line = keyResult.getData();
                lines.push_back(line);
            }
        }
    }
    else {
        return Result<std::vector<std::string>>::error("Failed to open file: " + std::string(strerror(errno)));
    }
    return Result<std::vector<std::string>>::ok(lines);
}

std::ostream &MainClass::cout()
{
    return _outputStream == 0 ? std::cout : *_outputStream;
}
