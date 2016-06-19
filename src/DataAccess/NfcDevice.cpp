//
// Created by robin on 6/19/16.
//

#include "NfcDevice.h"

NfcDevice::NfcDevice(LibNfc* libNfc, std::string str)
    : _connStr(str)
    , _device(0)
    , _libNfc(libNfc)
{
}

NfcDevice::~NfcDevice()
{
    close();
}

ResultBool NfcDevice::open()
{
    if (isOpened()) {
        return ResultBool::error("NFC device is already opened");
    }
    _device = nfc_open(_libNfc->getContext(), _connStr.c_str());
    if (!_device) {
        return ResultBool::error("Failed to open NFC device");
    }
    return ResultBool::ok(true);
}

bool NfcDevice::isOpened()
{
    return _device != 0;
}

void NfcDevice::close()
{
    if (isOpened()) {
        nfc_close(_device);
        _device = 0;
    }
}
