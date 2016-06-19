//
// Created by robin on 6/19/16.
//

#include "LibNfc.h"

LibNfc::LibNfc()
    : _context(0)
{
}

ResultBool LibNfc::init()
{
    if (isInitialized()) {
        return ResultBool::error("LibNfc is already initialized");
    }
    nfc_init(&_context);
    if (!_context) {
        return ResultBool::error("LibNfc could not be initialized");
    }
    return ResultBool::ok(true);
}

std::string LibNfc::getVersion()
{
    return nfc_version();
}

LibNfc::~LibNfc()
{
    clean();
}

bool LibNfc::isInitialized()
{
    return _context != 0;
}

void LibNfc::clean()
{
    if (isInitialized()) {
        nfc_exit(_context);
        _context = 0;
    }
}

Result<std::vector<NfcDevice*>> LibNfc::getDevices()
{
    if (!isInitialized()) {
        return Result<std::vector<NfcDevice*>>::error("LibNfc is not initialized");
    }
    nfc_connstring devices[16];
    size_t count = nfc_list_devices(_context, devices, sizeof(devices));
    if (count < 0) {
        return Result<std::vector<NfcDevice*>>::error("Failed to list NFC devices");
    }
    std::vector<NfcDevice*> devicesList;
    for (size_t i = 0; i < count; ++i) {
        devicesList.push_back(new NfcDevice(this, devices[i]));
    }
    return Result<std::vector<NfcDevice*>>::ok(devicesList);
}

nfc_context *LibNfc::getContext() const
{
    return _context;
}
