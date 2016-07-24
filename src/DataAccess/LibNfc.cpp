//
// Created by robin on 6/19/16.
//

#include <boost/shared_ptr.hpp>
#include "LibNfc.h"

LibNfc::LibNfc()
    : _context(0)
{
}

LibNfc::~LibNfc()
{
    clean();
}

ResultBool LibNfc::init()
{
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

void LibNfc::clean()
{
    nfc_exit(_context);
    _context = 0;
}

Result<std::vector<std::shared_ptr<NfcDevice>>> LibNfc::getDevices() const
{
    nfc_connstring devices[16];
    memset((char*)devices, 0, sizeof(devices));
    size_t count = nfc_list_devices(_context, devices, 16);
    if (count < 0) {
        return Result<std::vector<std::shared_ptr<NfcDevice>>>::error("Failed to list NFC devices: " + count);
    }
    std::vector<std::shared_ptr<NfcDevice>> devicesList;
    for (size_t i = 0; i < count; ++i) {
        devicesList.push_back(std::make_shared<NfcDevice>(this, devices[i]));
    }
    return Result<std::vector<std::shared_ptr<NfcDevice>>>::ok(devicesList);
}

nfc_context *LibNfc::getContext() const
{
    return _context;
}
