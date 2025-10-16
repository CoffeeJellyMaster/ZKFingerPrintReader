#include "raylib.h"
#include "FingerprintDevice.h"
#include <string>
#include <vector>
#include <sstream>

// Utility function for button drawing
bool DrawButton(const char* text, Rectangle rect, Color color, int fontSize = 20) {
    DrawRectangleRec(rect, color);
    DrawText(text, rect.x + 10, rect.y + 10, fontSize, WHITE);
    return (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && CheckCollisionPointRec(GetMousePosition(), rect));
}

void RunGuiDemo() {
    const int screenWidth = 1000;
    const int screenHeight = 650;

    InitWindow(screenWidth, screenHeight, "ZKTeco Fingerprint SDK + Raylib GUI");
    SetTargetFPS(60);

    FingerprintDevice fp;
    std::string statusMessage = "Idle.";
    std::string errorLog = "";
    std::string debugInfo = ""; // <- new

    bool deviceOpen = false;
    Image liveImage = { 0 };
    Texture2D liveTexture = { 0 };
    bool hasLiveImage = false;

    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(RAYWHITE);

        DrawText("ZKTeco Fingerprint Demo", 180, 40, 30, DARKGRAY);

        // ==== Buttons Column (Left side) ====
        if (DrawButton("Connect", {100, 120, 150, 40}, GREEN)) {
            debugInfo = "Attempting to initialize SDK...\n";

            if (fp.initialize()) {
                debugInfo += "SDK initialized successfully.\n";

                int count = fp.getDeviceCount();
                std::ostringstream oss;
                oss << "Device count: " << count << "\n";
                debugInfo += oss.str();

                if (count <= 0) {
                    statusMessage = "No fingerprint devices detected.";
                    errorLog = "Error: No device found.";
                    debugInfo += "No devices detected.\n";
                } else if (fp.openDevice(0)) {
                    statusMessage = "Device connected successfully.";
                    deviceOpen = true;
                    errorLog.clear();
                    debugInfo += "Device 0 opened successfully.\n";
                } else {
                    statusMessage = "Failed to open device.";
                    errorLog = fp.getLastError();
                    debugInfo += "Device open failed: " + errorLog + "\n";
                }
            } else {
                statusMessage = "SDK initialization failed.";
                errorLog = fp.getLastError();
                debugInfo += "SDK initialization failed: " + errorLog + "\n";
            }
        }

        if (DrawButton("Disconnect", {300, 120, 150, 40}, RED)) {
            fp.closeDevice();
            fp.terminate();
            deviceOpen = false;
            statusMessage = "Device disconnected.";
            errorLog.clear();
            debugInfo += "Device disconnected.\n";
        }

        // ==== Row 2 ====
        if (DrawButton("Register", {100, 200, 150, 40}, BLUE)) {
            if (!deviceOpen) errorLog = "Device not connected.";
            else {
                statusMessage = "Registering fingerprint...";
                if (!fp.registerFingerprint()) errorLog = fp.getLastError();
                else errorLog.clear();
            }
        }

        if (DrawButton("Clear", {300, 200, 150, 40}, ORANGE)) {
            if (!deviceOpen) errorLog = "Device not connected.";
            else {
                statusMessage = "Clearing fingerprints...";
                if (!fp.clearFingerprints()) errorLog = fp.getLastError();
                else errorLog.clear();
            }
        }

        // ==== Row 3 ====
        if (DrawButton("Verify", {100, 280, 150, 40}, DARKBLUE)) {
            if (!deviceOpen) errorLog = "Device not connected.";
            else {
                statusMessage = "Verifying fingerprint...";
                if (!fp.verifyFingerprint()) errorLog = fp.getLastError();
                else errorLog.clear();
            }
        }

        if (DrawButton("Identify", {300, 280, 150, 40}, DARKGREEN)) {
            if (!deviceOpen) errorLog = "Device not connected.";
            else {
                statusMessage = "Identifying fingerprint...";
                if (!fp.identifyFingerprint()) errorLog = fp.getLastError();
                else errorLog.clear();
            }
        }

        // ==== Row 4 ====
        if (DrawButton("Register by Image", {100, 360, 150, 40}, GRAY)) {
            if (!deviceOpen) errorLog = "Device not connected.";
            else {
                statusMessage = "Register by image...";
                if (!fp.registerByImage("finger.bmp")) errorLog = fp.getLastError();
                else errorLog.clear();
            }
        }

        if (DrawButton("Identify by Image", {300, 360, 150, 40}, DARKGRAY)) {
            if (!deviceOpen) errorLog = "Device not connected.";
            else {
                statusMessage = "Identify by image...";
                if (!fp.identifyByImage("finger.bmp")) errorLog = fp.getLastError();
                else errorLog.clear();
            }
        }

        // ==== Row 5: Acquire Live Fingerprint ====
        if (DrawButton("Acquire Live Fingerprint", {150, 440, 250, 50}, PURPLE)) {
            if (!deviceOpen) errorLog = "Device not connected.";
            else {
                statusMessage = "Capturing live fingerprint...";
                std::vector<unsigned char> img;
                int width = 0, height = 0;
                if (fp.acquireLiveFingerprint(img, width, height)) {
                    // Update texture
                    if (hasLiveImage) UnloadTexture(liveTexture);
                    liveImage = {
                        .data = img.data(),
                        .width = width,
                        .height = height,
                        .mipmaps = 1,
                        .format = PIXELFORMAT_UNCOMPRESSED_GRAYSCALE
                    };
                    liveTexture = LoadTextureFromImage(liveImage);
                    hasLiveImage = true;
                    statusMessage = "Live fingerprint captured.";
                    errorLog.clear();
                } else {
                    errorLog = fp.getLastError();
                }
            }
        }

        // ==== Right Column: Live Image ====
        DrawRectangleLines(600, 120, 300, 300, GRAY);
        DrawText("Live Fingerprint", 650, 90, 20, DARKGRAY);
        if (hasLiveImage) {
            DrawTextureEx(liveTexture, {610, 130}, 0.0f, 1.5f, WHITE);
        } else {
            DrawText("No image captured.", 640, 250, 18, LIGHTGRAY);
        }

        // ==== Status and Logs ====
        DrawText(("Status: " + statusMessage).c_str(), 100, 520, 20, BLACK);
        DrawText(("Error: " + errorLog).c_str(), 600, 450, 16, RED);

        // ==== Debug Info Box ====
        DrawRectangleLines(100, 560, 800, 70, DARKGRAY);
        DrawText("Debug Info:", 110, 570, 18, DARKGRAY);
        DrawText(debugInfo.c_str(), 110, 590, 18, GRAY);

        EndDrawing();
    }

    if (hasLiveImage) UnloadTexture(liveTexture);
    if (deviceOpen) fp.closeDevice();
    fp.terminate();
    CloseWindow();
}
