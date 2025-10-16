#include "FingerprintDevice.h"
#include <iostream>

FingerprintDevice::FingerprintDevice() = default;

FingerprintDevice::~FingerprintDevice() {
    closeDevice();
    terminate();
}

bool FingerprintDevice::initialize() {
    int result = ZKFPM_Init();
    if (result != ZKFP_ERR_OK) {
        lastError = "Failed to initialize SDK. Error code: " + std::to_string(result);
        return false;
    }
    initialized = true;
    dbCache = ZKFPM_DBInit();
    if (!dbCache) {
        lastError = "Failed to create fingerprint DB cache.";
        return false;
    }
    return true;
}

void FingerprintDevice::terminate() {
    if (dbCache) {
        ZKFPM_DBFree(dbCache);
        dbCache = nullptr;
    }
    if (initialized) {
        ZKFPM_Terminate();
        initialized = false;
    }
}

int FingerprintDevice::getDeviceCount() const {
    if (!initialized) return -1;
    return ZKFPM_GetDeviceCount();
}

bool FingerprintDevice::openDevice(int index) {
    if (!initialized) {
        lastError = "SDK not initialized.";
        return false;
    }

    deviceHandle = ZKFPM_OpenDevice(index);
    if (!deviceHandle) {
        lastError = "Failed to open device.";
        return false;
    }
    return true;
}

void FingerprintDevice::closeDevice() {
    if (deviceHandle) {
        ZKFPM_CloseDevice(deviceHandle);
        deviceHandle = nullptr;
    }
}

std::string FingerprintDevice::getLastError() const {
    return lastError;
}

// ===== Extended SDK operations =====

bool FingerprintDevice::registerFingerprint() {
    if (!deviceHandle) {
        lastError = "Device not opened.";
        return false;
    }
    lastError = "Register() not implemented yet.";
    return false;
}

bool FingerprintDevice::clearFingerprints() {
    if (!dbCache) {
        lastError = "DB cache not available.";
        return false;
    }
    int res = ZKFPM_DBClear(dbCache);
    if (res != ZKFP_ERR_OK) {
        lastError = "Failed to clear fingerprints. Error code: " + std::to_string(res);
        return false;
    }
    return true;
}

bool FingerprintDevice::verifyFingerprint() {
    if (!deviceHandle) {
        lastError = "Device not opened.";
        return false;
    }
    lastError = "Verify() not implemented yet.";
    return false;
}

bool FingerprintDevice::identifyFingerprint() {
    if (!deviceHandle) {
        lastError = "Device not opened.";
        return false;
    }
    lastError = "Identify() not implemented yet.";
    return false;
}

bool FingerprintDevice::registerByImage(const std::string& imagePath) {
    if (!dbCache) {
        lastError = "DB cache not available.";
        return false;
    }
    unsigned char templateBuf[2048];
    unsigned int templateSize = sizeof(templateBuf);
    int res = ZKFPM_ExtractFromImage(dbCache, imagePath.c_str(), 500, templateBuf, &templateSize);
    if (res != ZKFP_ERR_OK) {
        lastError = "Failed to register by image. Error code: " + std::to_string(res);
        return false;
    }
    return true;
}

bool FingerprintDevice::identifyByImage(const std::string& imagePath) {
    if (!dbCache) {
        lastError = "DB cache not available.";
        return false;
    }
    unsigned char templateBuf[2048];
    unsigned int templateSize = sizeof(templateBuf);
    int res = ZKFPM_ExtractFromImage(dbCache, imagePath.c_str(), 500, templateBuf, &templateSize);
    if (res != ZKFP_ERR_OK) {
        lastError = "Failed to extract fingerprint image for identification.";
        return false;
    }
    lastError = "IdentifyByImage() not implemented yet.";
    return false;
}

// ===== Live Fingerprint Capture =====
// bool FingerprintDevice::acquireLiveFingerprint(std::vector<unsigned char>& imageBuffer, int& width, int& height) {
//     if (!deviceHandle) {
//         lastError = "Device not opened.";
//         return false;
//     }

//     unsigned char paramBuf[4];
//     unsigned int size = 4;
//     if (ZKFPM_GetParameters(deviceHandle, 1, paramBuf, &size) != ZKFP_ERR_OK) {
//         lastError = "Failed to get image width.";
//         return false;
//     }
//     width = *(int*)paramBuf;

//     if (ZKFPM_GetParameters(deviceHandle, 2, paramBuf, &size) != ZKFP_ERR_OK) {
//         lastError = "Failed to get image height.";
//         return false;
//     }
//     height = *(int*)paramBuf;

//     unsigned int imgSize = width * height;
//     imageBuffer.resize(imgSize);
//     unsigned char fpTemplate[2048];
//     unsigned int templateSize = sizeof(fpTemplate);

//     int res = ZKFPM_AcquireFingerprint(deviceHandle, imageBuffer.data(), imgSize, fpTemplate, &templateSize);
//     if (res != ZKFP_ERR_OK) {
//         lastError = "Failed to acquire fingerprint. Error code: " + std::to_string(res);
//         return false;
//     }

//     return true;
// }
bool FingerprintDevice::acquireLiveFingerprint(std::vector<unsigned char>& imageBuffer, int& width, int& height) {
    if (!deviceHandle) {
        lastError = "Device not opened.";
        return false;
    }

    unsigned char paramBuf[4];
    unsigned int size = 4;

    if (ZKFPM_GetParameters(deviceHandle, 1, paramBuf, &size) != ZKFP_ERR_OK) {
        lastError = "Failed to get image width.";
        return false;
    }
    width = *(int*)paramBuf;

    if (ZKFPM_GetParameters(deviceHandle, 2, paramBuf, &size) != ZKFP_ERR_OK) {
        lastError = "Failed to get image height.";
        return false;
    }
    height = *(int*)paramBuf;

    unsigned int imgSize = width * height;
    imageBuffer.resize(imgSize);

    unsigned char fpTemplate[2048];
    unsigned int templateSize = sizeof(fpTemplate);

    int res = ZKFPM_AcquireFingerprint(deviceHandle, imageBuffer.data(), imgSize, fpTemplate, &templateSize);
    if (res != ZKFP_ERR_OK) {
        lastError = "Failed to acquire fingerprint. Error code: " + std::to_string(res);
        return false;
    }

    // 🟣 Convert fingerprint template to HEX string
    std::string hexTemplate;
    char buf[3];
    for (unsigned int i = 0; i < templateSize; ++i) {
        sprintf(buf, "%02X", fpTemplate[i]);
        hexTemplate += buf;
    }

// Inside acquireLiveFingerprint after converting to HEX:
lastHexTemplate = hexTemplate;


    return true;
}
