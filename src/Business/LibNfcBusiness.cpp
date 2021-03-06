//
// Created by robin on 7/22/16.
//

#include <algorithm>
#include "LibNfcBusiness.h"

#define Q(x) #x
#define QUOTE(x) Q(x)

#ifndef GIT_SHA1
#define GIT_SHA1 "unknown"
#endif

#ifndef GIT_REF_NAME
#define GIT_REF_NAME "unknown"
#endif

LibNfcBusiness::LibNfcBusiness()
{
    _libNfc = std::make_shared<LibNfc>();
}

LibNfcBusiness::~LibNfcBusiness()
{
    _libNfc->clean();
}

ResultBool LibNfcBusiness::init()
{
    if (isInitialized()) {
        return ResultBool::error("LibNfc is already initialized");
    }
    return _libNfc->init();
}

std::string LibNfcBusiness::getLibNfcVersion()
{
    return LibNfc::getVersion();
}

void LibNfcBusiness::clean()
{
    if (isInitialized()) {
        _libNfc->clean();
    }
}

bool LibNfcBusiness::isInitialized() const
{
    return _libNfc->getContext() != 0;
}

Result<std::vector<std::shared_ptr<NfcDeviceBusiness>>> LibNfcBusiness::getDevices() const
{
    if (!isInitialized()) {
        return Result<std::vector<std::shared_ptr<NfcDeviceBusiness>>>::error("LibNfc is not initialized");
    }
    auto devicesResult = _libNfc->getDevices();
    if (!devicesResult) {
        return Result<std::vector<std::shared_ptr<NfcDeviceBusiness>>>::error(devicesResult);
    }
    auto devices = devicesResult.getData();
    std::vector<std::shared_ptr<NfcDeviceBusiness>> devicesBusiness;
    for (auto device : devices) {
        devicesBusiness.push_back(std::make_shared<NfcDeviceBusiness>(device));
    }
    return Result<std::vector<std::shared_ptr<NfcDeviceBusiness>>>::ok(devicesBusiness);
}

std::shared_ptr<LibNfc> LibNfcBusiness::getLibNfc() const
{
    return _libNfc;
}

std::string LibNfcBusiness::getMifareToolsVersion()
{
    return QUOTE(GIT_REF_NAME)  "-"  QUOTE(GIT_SHA1);
}
