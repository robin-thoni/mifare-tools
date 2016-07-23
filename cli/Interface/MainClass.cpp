//
// Created by robin on 8/8/15.
//

#include <iostream>
#include <sysexits.h>
#include <DBO/StringUtils.h>
#include <Business/FreeFareDeviceBusiness.h>
#include <iomanip>
#include "DBO/Result.h"
#include "Business/LibNfcBusiness.h"
#include "CommandLineParser.h"
#include "MainClass.h"

MainClass::MainClass(int argc, char *argv[])
    : _argc(argc)
    , _argv(argv)
{
}

int MainClass::main()
{
    std::cout << "LibNfc version: " << LibNfcBusiness::getVersion() << std::endl;

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
    if (devices.size() == 0) {
        std::cerr << "No NFC device found" << std::endl;
        return 3;
    }

    std::cout << "Found " << devices.size() << " devices: " << std::endl;
    for (size_t i = 0; i < devices.size(); ++i) {
        std::cout << devices[i]->getConnStr() << std::endl;
    }
    std::cout << "Using first device" << std::endl;

    auto device = devices[0];
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

    std::cout << "Found " << tagsResult.getData().size() << " tags:" << std::endl;
    for (size_t i = 0; i < tagsResult.getData().size(); ++i) {
        auto tag = tagsResult.getData()[i];
        std::cout << "UID: " << tag->getUid() << std::endl;
        std::cout << "Type: " << tag->getType() << std::endl;
    }
    std::cout << "Using first tag" << std::endl;

    auto tag = tags[0];

    std::vector<std::string> keys;
//    keys.push_back(StringUtils::humanToRaw("8829da9daf76").getData());
//    keys.push_back(StringUtils::humanToRaw("ffffffffffff").getData());
    keys.push_back(StringUtils::humanToRaw("484558414354").getData());

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
    std::cout << "\r" << std::fixed << std::setprecision(1) <<  ((float)done / (float)total * 100.0) << "%" << std::flush;
}
