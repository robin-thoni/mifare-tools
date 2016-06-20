//
// Created by robin on 6/19/16.
//

#include "FreeFareDevice.h"

FreeFareDevice::FreeFareDevice(std::shared_ptr<NfcDevice> device)
    : _device(device)
{
}

Result<std::vector<std::shared_ptr<FreeFareTag>>> FreeFareDevice::getTags()
{
    if (!_device->isOpened()) {
        return Result<std::vector<std::shared_ptr<FreeFareTag>>>::error("Device is not opened");
    }
    FreefareTag* tags = freefare_get_tags(_device->getDevice());
    if (!tags) {
        return Result<std::vector<std::shared_ptr<FreeFareTag>>>::error("Failed to get MIFARE tags");
    }
    std::vector<std::shared_ptr<FreeFareTag>> tagList;
    for (size_t i = 0; tags[i] != 0; ++i) {
        tagList.push_back(std::make_shared<FreeFareTag>(tags[i]));
    }
    return Result<std::vector<std::shared_ptr<FreeFareTag>>>::ok(tagList);
}
