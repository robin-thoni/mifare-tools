//
// Created by robin on 7/22/16.
//

#ifndef MIFARE_TOOLS_NFCDEVICEBUSINESS_H
#define MIFARE_TOOLS_NFCDEVICEBUSINESS_H


#include <DataAccess/NfcDevice.h>

class NfcDeviceBusiness
{
public:
    NfcDeviceBusiness(std::shared_ptr<NfcDevice> device);

    ResultBool open();

    void close();

    bool isOpened() const;

    const std::string & getConnStr() const;

    std::shared_ptr<NfcDevice> getDevice() const;

protected:
    std::shared_ptr<NfcDevice> _device;
};


#endif //MIFARE_TOOLS_NFCDEVICEBUSINESS_H
