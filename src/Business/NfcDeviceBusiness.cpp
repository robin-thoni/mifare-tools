//
// Created by robin on 7/22/16.
//

#include "NfcDeviceBusiness.h"

NfcDeviceBusiness::NfcDeviceBusiness(std::shared_ptr<NfcDevice> device)
    : _device(device)
{
}

ResultBool NfcDeviceBusiness::open()
{
    if (isOpened()) {
        return ResultBool::error("NFC device is already opened");
    }
    return _device->open();
}

void NfcDeviceBusiness::close()
{
    if (isOpened()) {
        _device->close();
    }
}

bool NfcDeviceBusiness::isOpened() const
{
    return _device->getDevice() != 0;
}

const std::string &NfcDeviceBusiness::getConnStr() const
{
    return _device->getConnStr();
}

std::shared_ptr<NfcDevice> NfcDeviceBusiness::getDevice() const
{
    return _device;
}
