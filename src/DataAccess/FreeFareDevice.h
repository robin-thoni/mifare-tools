//
// Created by robin on 6/19/16.
//

#ifndef MIFARE_TOOLS_FREEFAREDEVICE_H
#define MIFARE_TOOLS_FREEFAREDEVICE_H


#include "NfcDevice.h"
#include <freefare.h>

class FreeFareDevice
{
public:
    FreeFareDevice(NfcDevice* device);

private:
    NfcDevice* _device;
};


#endif //MIFARE_TOOLS_FREEFAREDEVICE_H
