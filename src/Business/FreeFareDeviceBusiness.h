//
// Created by robin on 7/22/16.
//

#ifndef MIFARE_TOOLS_FREEFAREDEVICEBUSINESS_H
#define MIFARE_TOOLS_FREEFAREDEVICEBUSINESS_H


#include <DataAccess/FreeFareDevice.h>
#include "NfcDeviceBusiness.h"
#include "FreeFareTagBusiness.h"

class FreeFareDeviceBusiness
{
public:
    FreeFareDeviceBusiness(std::shared_ptr<NfcDeviceBusiness> device);

    Result<std::vector<std::shared_ptr<FreeFareTagBusiness>>> getTags() const;

protected:
    std::shared_ptr<NfcDeviceBusiness> _device;

    std::shared_ptr<FreeFareDevice> _freeFareDevice;
};


#endif //MIFARE_TOOLS_FREEFAREDEVICEBUSINESS_H
