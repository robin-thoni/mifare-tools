//
// Created by robin on 6/19/16.
//

#ifndef MIFARE_TOOLS_LIBNFC_H
#define MIFARE_TOOLS_LIBNFC_H

#include <memory>
#include <nfc/nfc.h>
#include "NfcDevice.h"
#include "DBO/Result.h"

class NfcDevice;

class LibNfc
{
public:
    LibNfc();
    ~LibNfc();

    ResultBool init();

    void clean();

    bool isInitialized();

    Result<std::vector<std::shared_ptr<NfcDevice>>> getDevices();

    static std::string getVersion();

    nfc_context* getContext() const;

private:
    nfc_context* _context;
};


#endif //MIFARE_TOOLS_LIBNFC_H
