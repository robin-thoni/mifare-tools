//
// Created by robin on 8/8/15.
//

#include <iostream>
#include <sysexits.h>
#include <DBO/Result.h>
#include <DataAccess/LibNfc.h>
#include <DataAccess/FreeFareDevice.h>
#include "CommandLineParser.h"
#include "MainClass.h"

MainClass::MainClass(int argc, char *argv[])
    : _argc(argc)
    , _argv(argv)
{
}

int MainClass::main()
{
    std::cout << "LibNfc version: " << LibNfc::getVersion() << std::endl;

    LibNfc libNfc;
    auto init = libNfc.init();
    if (!init) {
        init.print();
        return 1;
    }

    auto devices = libNfc.getDevices();
    if (!devices) {
        devices.print();
        return 2;
    }
    if (devices.getData().size() == 0) {
        std::cerr << "No NFC device found" << std::endl;
        return 3;
    }

    std::cout << "Found " << devices.getData().size() << " devices: " << std::endl;
    for (size_t i = 0; i < devices.getData().size(); ++i) {
        std::cout << devices.getData()[i]->getConnStr() << std::endl;
    }

    auto device = devices.getData()[0];
    auto open = device->open();
    if (!open) {
        open.print();
        return 4;
    }

    FreeFareDevice freeFareDevice(device);
    auto tags = freeFareDevice.getTags();
    if (!tags) {
        tags.print();
        return 5;
    }

    std::cout << "Found " << tags.getData().size() << " tags:" << std::endl;
    for (size_t i = 0; i < tags.getData().size(); ++i) {
        auto tag = tags.getData()[i];
        std::cout << "UID: " << tag->getUid() << std::endl;

    }

    device->close();

    libNfc.clean();

    return 0;
}
