#pragma once
#define WIN32_LEAN_AND_MEAN
#define NOGDI
#define NOUSER
#include <windows.h>
#include <string>
#include <vector>
#include "libzkfp.h"
#include "libzkfperrdef.h"

class FingerprintDevice {
public:
    FingerprintDevice();
    ~FingerprintDevice();

    bool initialize();
    void terminate();
    int getDeviceCount() const;
    bool openDevice(int index = 0);
    void closeDevice();
    std::string getLastError() const;

    // Extended operations
    bool registerFingerprint();
    bool clearFingerprints();
    bool verifyFingerprint();
    bool identifyFingerprint();
    bool registerByImage(const std::string& imagePath);
    bool identifyByImage(const std::string& imagePath);
    bool acquireLiveFingerprint(std::vector<unsigned char>& imageBuffer, int& width, int& height);
    inline HANDLE getHandle() const { return deviceHandle; }

private:
    HANDLE deviceHandle = nullptr;
    HANDLE dbCache = nullptr;
    bool initialized = false;
    std::string lastError;
};
