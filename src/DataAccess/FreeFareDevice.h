//
// Created by robin on 6/19/16.
//

#ifndef MIFARE_TOOLS_FREEFAREDEVICE_H
#define MIFARE_TOOLS_FREEFAREDEVICE_H


#include <freefare.h>
#include "NfcDevice.h"
#include "FreeFareTag.h"

class FreeFareDevice
{
public:
    FreeFareDevice(std::shared_ptr<NfcDevice> device);

    Result<std::vector<std::shared_ptr<FreeFareTag>>> getTags();

private:
    std::shared_ptr<NfcDevice> _device;
};


#endif //MIFARE_TOOLS_FREEFAREDEVICE_H
