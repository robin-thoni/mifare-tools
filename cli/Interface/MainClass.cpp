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

MainClass::MainClass(int argc, char *argv[])
    : _argc(argc)
    , _argv(argv)
{
}

#define Q(x) #x
#define QUOTE(x) Q(x)

#ifndef GIT_SHA1
#define GIT_SHA1 "unknown"
#endif

#ifndef GIT_REF_NAME
#define GIT_REF_NAME "unknown"
#endif

int MainClass::main()
{
    CommandLineParser parser(_argc, _argv);
    CommandLineOption optionVersion(&parser, "version", 'v', "Show libnfc and mifare-tools versions");
    CommandLineOption optionHelp(&parser, "help", 'h', "Show this help");

    CommandLineOption optionDevice(&parser, "device", 'd', "Use the device DEVICE", "DEVICE");
    CommandLineOption optionUid(&parser, "uid", 'u', "Use the UID tag", "UID");

    CommandLineOption optionKeyFile(&parser, "key-file", 'f', "Path to a file containing keys", "FILE");

    if (!parser.parse()) {
        return parser.showHelp(EX_USAGE);
    }
    if (optionVersion.isSet()) {
        printVersion();
        return 0;
    }
    if (optionHelp.isSet()) {
        return parser.showHelp(0, false);
    }

    std::string deviceName = "";
    if (!optionDevice.isSet()) {
        deviceName = optionDevice.getValue();
    }

    std::string tagUid = "";
    if (!optionUid.isSet()) {
        tagUid = optionUid.getValue();
    }

    std::vector<std::string> keys;
    if (optionKeyFile.isSet()) {
        for (auto filePath : optionKeyFile.getValues()) {
            auto keysResult = readFile(filePath);
            if (!keysResult) {
                keysResult.print();
                return 1;
            }
            auto fileKeys = keysResult.getData();
            keys.insert(keys.end(), fileKeys.begin(), fileKeys.end());
        }
    }

    LibNfcBusiness libNfc;
    auto init = libNfc.init();
    if (!init) {
        init.print();
        return 1;
    }

    auto devicesResult = libNfc.getDevices();
    if (!devicesResult) {
        devicesResult.print();
        return 2;
    }
    auto devices = devicesResult.getData();
    std::shared_ptr<NfcDeviceBusiness> device = getDevice(deviceName, devices);
    if (device == 0) {
        std::cerr << "NFC device not found" << std::endl;
        return 3;
    }
    auto open = device->open();
    if (!open) {
        open.print();
        return 4;
    }

    FreeFareDeviceBusiness freeFareDevice(device);
    auto tagsResult = freeFareDevice.getTags();
    if (!tagsResult) {
        tagsResult.print();
        return 5;
    }

    auto tags = tagsResult.getData();
    if (tags.size() == 0) {
        std::cerr << "No tag found" << std::endl;
        return 6;
    }

    auto tag = getTag(tagUid, tags);
    if (tag == 0) {
        std::cerr << "Tag not found" << std::endl;
        return 6;
    }

//    std::vector<std::string> keys;
//    keys.push_back(StringUtils::humanToRaw("8829da9daf76").getData());
//    keys.push_back(StringUtils::humanToRaw("ffffffffffff").getData());
//    keys.push_back(StringUtils::humanToRaw("484558414354").getData());

    int res = dump(tag, keys);
//    int res = mapKeys(tag, keys);

    device->close();
    libNfc.clean();

    return res;
}

int MainClass::mapKeys(std::shared_ptr<FreeFareTagBusiness> tag, std::vector<std::string> keys)
{
    auto mappedKeysResult = tag->mapKeys(keys, printPercent);
    std::cout << "\r";
    if (!mappedKeysResult) {
        mappedKeysResult.print();
    }
    else {
        auto mappedKeys = mappedKeysResult.getData();
        for (int s = 0; s < mappedKeys.size(); ++s) {
            auto sectorKeys = mappedKeys[s];
            std::cout << "+Sector: " << s << std::endl;
            for (int b = 0; b < sectorKeys.size(); ++b) {
                auto blockKeys = sectorKeys[b];
                std::cout << "+Block: " << b << std::endl;
                std::cout << "Key A: " << StringUtils::rawToHuman(blockKeys.first) << std::endl;
                std::cout << "Key B: " << StringUtils::rawToHuman(blockKeys.second) << std::endl;
            }
        }
    }
    return 0;
}

int MainClass::dump(std::shared_ptr<FreeFareTagBusiness> tag, std::vector<std::string> keys)
{
    auto dumpResult = tag->dump(keys, printPercent, printPercent);
    std::cout << "\r";
    if (!dumpResult) {
        dumpResult.print();
        return 7;
    }
    auto dump = dumpResult.getData();
    for(int s = 0; s < 16; ++s) {
        std::cout << "+Sector: " << s << std::endl;
        auto sector = dump[s];
        for (int b = 0; b < 3; ++b) {
            std::cout << (sector.hasBlock(b) ? StringUtils::rawToHuman(sector.getBlock(b)) : std::string(32, '-')) << std::endl;
        }
        std::cout << "" << (sector.hasKeyA() ? StringUtils::rawToHuman(sector.getKeyA()) : std::string(12, '-'))
        << (sector.hasAccessBits() ? StringUtils::rawToHuman(sector.getAccessBits()) : std::string(8, '-'))
        << (sector.hasKeyB() ? StringUtils::rawToHuman(sector.getKeyB()) : std::string(12, '-')) << std::endl;


        std::cout << "+Trailer key A: " << (sector.hasKeyA() ? StringUtils::rawToHuman(sector.getKeyA()) : std::string(12, '-'))
            << "\t AC bits: " << (sector.hasAccessBits() ? StringUtils::rawToHuman(sector.getAccessBits()) : std::string(8, '-'))
            << "\t key B: " << (sector.hasKeyB() ? StringUtils::rawToHuman(sector.getKeyB()) : std::string(12, '-')) << std::endl;
        AccessBitsDbo accessBitsDbo = sector.getAccessBitsDbo();
        for (int b = 0; b < 3; ++b) {
            std::cout << "+Block: " << b << " ";
            printBlockAccessBits(accessBitsDbo, b);
        }
        std::cout << "+Block: 4 ";
        printTrailerAccessBits(accessBitsDbo);
    }
    return 0;
}

void MainClass::printBlockAccessBits(const AccessBitsDbo &accessBits, int block)
{
    std::cout << "read: " << (accessBits.canKeyAReadBlock(block) ? "A" : "") << (accessBits.canKeyBReadBlock(block) ? "B" : "");
    std::cout << "\t write: " << (accessBits.canKeyAWriteBlock(block) ? "A" : "") << (accessBits.canKeyBWriteBlock(block) ? "B" : "");
    std::cout << "\t increment: " << (accessBits.canKeyAIncrementBlock(block) ? "A" : "") << (accessBits.canKeyBIncrementBlock(block) ? "B" : "");
    std::cout << "\t decrement: " << (accessBits.canKeyADecrementBlock(block) ? "A" : "") << (accessBits.canKeyBDecrementBlock(block) ? "B" : "") << std::endl;
}

void MainClass::printTrailerAccessBits(const AccessBitsDbo &accessBits)
{
    std::cout << "key A read: " << (accessBits.canKeyAReadKeyATrailer() ? "A" : "") << (accessBits.canKeyBReadKeyATrailer() ? "B" : "");
    std::cout << "\t key A write: " << (accessBits.canKeyAWriteKeyATrailer() ? "A" : "") << (accessBits.canKeyBWriteKeyATrailer() ? "B" : "");

    std::cout << "\t AC bits read: " << (accessBits.canKeyAReadAccessBitsTrailer() ? "A" : "") << (accessBits.canKeyBReadAccessBitsTrailer() ? "B" : "");
    std::cout << "\t AC bits write: " << (accessBits.canKeyAWriteAccessBitsTrailer() ? "A" : "") << (accessBits.canKeyBWriteAccessBitsTrailer() ? "B" : "");

    std::cout << "\t key B read: " << (accessBits.canKeyAReadKeyBTrailer() ? "A" : "") << (accessBits.canKeyBReadKeyBTrailer() ? "B" : "");
    std::cout << "\t key B write: " << (accessBits.canKeyAWriteKeyBTrailer() ? "A" : "") << (accessBits.canKeyBWriteKeyBTrailer() ? "B" : "") << std::endl;;
}

void MainClass::printPercent(int done, int total)
{
    std::cout << "\r\033[2K" << std::fixed << std::setprecision(1) << ((float)done / (float)total * 100.0) << "%" << std::flush;
}

void MainClass::printVersion() const
{
    std::cout << "LibNfc version: " << LibNfcBusiness::getVersion() << std::endl;
    std::cout << "Mifare-tools version: " << QUOTE(GIT_REF_NAME) << "-" << QUOTE(GIT_SHA1) << std::endl;
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
