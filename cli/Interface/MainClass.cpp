//
// Created by robin on 8/8/15.
//

#include <iostream>
#include <sysexits.h>
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

    auto device = devices[0];
    auto open = device->open();
    if (!open) {
        open.print();
        return 4;
    }

    FreeFareDeviceBusiness freeFareDevice(device);
    auto tags = freeFareDevice.getTags();
    if (!tags) {
        tags.print();
        return 5;
    }

    std::cout << "Found " << tags.getData().size() << " tags:" << std::endl;
    for (size_t i = 0; i < tags.getData().size(); ++i) {
        auto tag = tags.getData()[i];
        std::cout << "UID: " << tag->getUid() << std::endl;
        std::cout << "Type: " << tag->getType() << std::endl;
        mapKeys(tag);
    }

    device->close();
    libNfc.clean();

    return 0;
}

int MainClass::mapKeys(std::shared_ptr<FreeFareTagBusiness> tag)
{
    std::vector<std::string> keys;
    keys.push_back(StringUtils::humanToRaw("8829da9daf76").getData());
    auto mappedKeysResult = tag->mapKeys(keys);
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

int MainClass::dump(std::shared_ptr<FreeFareTagBusiness> tag)
{
    for(int s = 0; s < 16; ++s)
    {
        std::cout << "+Sector: " << s << std::endl;
        auto sectorResult = tag->readSector(s, StringUtils::humanToRaw("8829da9daf76").getData(), MFC_KEY_A);
        if (!sectorResult)
        {
            sectorResult.print();
        }
        else
        {
            auto sector = sectorResult.getData();
            for (int b = 0; b < 3; ++b)
            {
                std::cout << StringUtils::rawToHuman(sector.getBlock(b)) << std::endl;
            }
            std::cout << StringUtils::rawToHuman(sector.getKeyA()) << " "
            << StringUtils::rawToHuman(sector.getAccessBits()) << " "
            << StringUtils::rawToHuman(sector.getKeyB()) << std::endl;
        }
    }
    return 0;
}


