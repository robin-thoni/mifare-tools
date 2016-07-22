//
// Created by robin on 7/22/16.
//

#include "FreeFareDeviceBusiness.h"

FreeFareDeviceBusiness::FreeFareDeviceBusiness(std::shared_ptr<NfcDeviceBusiness> device)
    : _device(device),
      _freeFareDevice(std::make_shared<FreeFareDevice>(_device->getDevice()))
{
}

Result<std::vector<std::shared_ptr<FreeFareTagBusiness>>> FreeFareDeviceBusiness::getTags() const
{
    if (!_device->isOpened()) {
        return Result<std::vector<std::shared_ptr<FreeFareTagBusiness>>>::error("Device is not opened");
    }
    auto tagsResult = _freeFareDevice->getTags();
    if (!tagsResult) {
        return Result<std::vector<std::shared_ptr<FreeFareTagBusiness>>>::error(tagsResult);
    }
    auto tags = tagsResult.getData();
    std::vector<std::shared_ptr<FreeFareTagBusiness>> tagsBusiness;
    for (auto tag : tags) {
        tagsBusiness.push_back(std::make_shared<FreeFareTagBusiness>(tag));
    }
    return Result<std::vector<std::shared_ptr<FreeFareTagBusiness>>>::ok(tagsBusiness);
}
