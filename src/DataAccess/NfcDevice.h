//
// Created by robin on 6/19/16.
//

#ifndef MIFARE_TOOLS_NFCDEVICEDBO_H
#define MIFARE_TOOLS_NFCDEVICEDBO_H


#include <string>
#include <nfc/nfc-types.h>
#include <DBO/Result.h>
#include "LibNfc.h"

class LibNfc;

class NfcDevice
{
public:
    NfcDevice(LibNfc* libNfc, std::string connStr);
    ~NfcDevice();

    ResultBool open();

    void close();

    bool isOpened();

    const std::string & getConnStr() const;

    void setConnStr(const std::string &connStr);

    nfc_device * getDevice() const;

private:
    std::string _connStr;

    nfc_device* _device;

    LibNfc* _libNfc;
};


#endif //MIFARE_TOOLS_NFCDEVICEDBO_H
