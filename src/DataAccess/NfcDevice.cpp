//
// Created by robin on 6/19/16.
//

#include "NfcDevice.h"

NfcDevice::NfcDevice(const LibNfc* libNfc, const std::string& str)
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
    _device = nfc_open(_libNfc->getContext(), _connStr.c_str());
    if (!_device) {
        return ResultBool::error("Failed to open NFC device");
    }
    return ResultBool::ok(true);
}

void NfcDevice::close()
{
    nfc_close(_device);
    _device = 0;
}

nfc_device *NfcDevice::getDevice() const
{
    return _device;
}

const std::string &NfcDevice::getConnStr() const
{
    return _connStr;
}
