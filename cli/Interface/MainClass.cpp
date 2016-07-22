//
// Created by robin on 8/8/15.
//

#include <iostream>
#include <sysexits.h>
#include <DBO/StringUtils.h>
#include <Business/FreeFareDeviceBusiness.h>
#include "DBO/Result.h"
#include "Business/LibNfcBusiness.h"
#include "Business/FreeFareDeviceBusiness.h"
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
        for(int i = 0; i < 16; ++i) {
            std::cout << "+Sector: " << i << std::endl;
            for (int j = 0; j < 4; ++j) {
                auto sector = tag->readBlock(i, j, StringUtils::humanToRaw("8829da9daf76").getData(), MFC_KEY_A);
                if (!sector) {
                    sector.print();
                }
                else {
                    std::cout << StringUtils::rawToHuman(sector.getData()) << std::endl;
                }
            }
        }
    }

    device->close();

    libNfc.clean();

    return 0;
}
