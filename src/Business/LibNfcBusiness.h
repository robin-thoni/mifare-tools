//
// Created by robin on 7/22/16.
//

#ifndef MIFARE_TOOLS_LIBNFCBUSINESS_H
#define MIFARE_TOOLS_LIBNFCBUSINESS_H


#include <DBO/Result.h>
#include <DataAccess/LibNfc.h>
#include "NfcDeviceBusiness.h"

class LibNfcBusiness
{
public:
    LibNfcBusiness();
    virtual ~LibNfcBusiness();

    ResultBool init();

    void clean();

    bool isInitialized() const;

    Result<std::vector<std::shared_ptr<NfcDeviceBusiness>>> getDevices() const;

    std::shared_ptr<LibNfc> getLibNfc() const;

    static std::string getVersion();

protected:
    std::shared_ptr<LibNfc> _libNfc;
};


#endif //MIFARE_TOOLS_LIBNFCBUSINESS_H
