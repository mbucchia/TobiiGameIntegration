// Unofficial bindings for the Tobii Game Integration SDK.
// Based on the Tobii Unity SDK for Desktop C# Interop.cs code.
// See the Tobii Licensing.

#pragma once

#include <Windows.h> // HWND, LoadLibrary(), GetProcAddress()
#include <shlwapi.h> // PathRemoveFileSpec()
#pragma comment(lib, "shlwapi.lib")
#include <strsafe.h> // StringCchCat()
#ifndef __cplusplus
#include <stdbool.h> // bool
#endif

enum TobiiSubscription {
    TobiiSubscriptionUserPresence = (1 << 1),
    TobiiSubscriptionStandardGaze = (1 << 2),
    TobiiSubscriptionFoveatedGaze = (1 << 3),
    TobiiSubscriptionHeadTracking = (1 << 4),
    TobiiSubscriptionWearableData = (1 << 5),

    TobiiSubscriptionForce32 = 0x7fffffff
};

enum UnitType {
    // gaze position, signed normalized, client window bottom, left = (-1, -1), client window top, right = (1, 1)
    SignedNormalized = 0,
    // gaze position, unsigned normalized, client window bottom, left = (0, 0), client window top, right = (1, 1)
    Normalized,
    // gaze position, mm, client window bottom, left = (0, 0), client window top, right = (window_width_mm,
    // window_height_mm)
    Mm,
    // gaze position, pixel, client window bottom, left = (0, 0), client window top, right = (window_width_pixels,
    // window_height_pixels
    Pixels,

    NumberOfUnitTypes
};

enum UserPresence { Unknown = 0, Away, Present };

#pragma pack(push, 1)
struct GazePoint {
    long long TimeStampMicroSeconds;
    float X;
    float Y;
};
#pragma pack(pop)

#pragma pack(push, 1)
struct HeadRotation {
    float Yaw;
    float Pitch;
    float Roll;
};
#pragma pack(pop)

#pragma pack(push, 1)
struct HeadPosition {
    float X;
    float Y;
    float Z;
};
#pragma pack(pop)

#pragma pack(push, 1)
struct HeadPose {
    long long TimeStampMicroSeconds;
    HeadRotation Rotation;
    HeadPosition Position;
};
#pragma pack(pop)

typedef bool (*tobii_pFnStart)(bool custom_thread);
typedef void (*tobii_pFnSetWindow)(HWND hWnd);
typedef void (*tobii_pFnStop)();
typedef void (*tobii_pFnSubscribeToStream)(enum TobiiSubscription stream);
typedef void (*tobii_pFnUnsubscribeFromStream)(enum TobiiSubscription stream);
typedef bool (*tobii_pFnUpdate)();
typedef void (*tobii_pFnGetNewGazePoints)(struct GazePoint** gazePoints,
                                          int* numberOfAvailableGazePoints,
                                          enum UnitType unitType);
typedef void (*tobii_pFnGetNewHeadPoses)(struct HeadPose** headPoses, int* numberOfAvailableHeadPoses);
typedef bool (*tobii_pFnIsInitialised)();
typedef bool (*tobii_pFnIsReady)();
typedef bool (*tobii_pFnIsConnected)();
typedef enum UserPresence (*tobii_pFnGetUserPresence)();
typedef bool (*tobii_pFnWasUpdated)();
typedef float (*tobii_pFnTimeSinceLastGazePacket)();
typedef float (*tobii_pFnTimeSinceLastHeadPacket)();
typedef void (*tobii_pFnGetScreenSizeMm)(int* width, int* height);

struct tobiiAPI {
    tobii_pFnStart Start;
    tobii_pFnSetWindow SetWindow;
    tobii_pFnStop Stop;
    tobii_pFnSubscribeToStream SubscribeToStream;
    tobii_pFnUnsubscribeFromStream UnsubscribeFromStream;
    tobii_pFnUpdate Update;
    tobii_pFnGetNewGazePoints GetNewGazePoints;
    tobii_pFnGetNewHeadPoses GetNewHeadPoses;
    tobii_pFnIsInitialised IsInitialised;
    tobii_pFnIsReady IsReady;
    tobii_pFnIsConnected IsConnected;
    tobii_pFnGetUserPresence GetUserPresence;
    tobii_pFnWasUpdated WasUpdated;
    tobii_pFnTimeSinceLastGazePacket TimeSinceLastGazePacket;
    tobii_pFnTimeSinceLastHeadPacket TimeSinceLastHeadPacket;
    tobii_pFnGetScreenSizeMm GetScreenSizeMm;
};

static bool InitializeTobiiAPI(struct tobiiAPI* api) {
    wchar_t dllPath[_MAX_PATH] = L".";

    HMODULE thisDLL;
    if (GetModuleHandleExA(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
                           (LPCSTR)&InitializeTobiiAPI,
                           &thisDLL)) {
        GetModuleFileNameW(thisDLL, dllPath, _MAX_PATH);
        PathRemoveFileSpecW(dllPath);
    }

    StringCchCatW(dllPath, _MAX_PATH, L"\\Tobii.GameIntegration.dll");
    const HMODULE tobiiLib = LoadLibraryW(dllPath);
    if (!tobiiLib) {
        return false;
    }

    api->Start = (tobii_pFnStart)GetProcAddress(tobiiLib, "Start");
    api->SetWindow = (tobii_pFnSetWindow)GetProcAddress(tobiiLib, "SetWindow");
    api->Stop = (tobii_pFnStop)GetProcAddress(tobiiLib, "Stop");
    api->SubscribeToStream = (tobii_pFnSubscribeToStream)GetProcAddress(tobiiLib, "SubscribeToStream");
    api->UnsubscribeFromStream = (tobii_pFnUnsubscribeFromStream)GetProcAddress(tobiiLib, "UnsubscribeFromStream");
    api->Update = (tobii_pFnUpdate)GetProcAddress(tobiiLib, "Update");
    api->GetNewGazePoints = (tobii_pFnGetNewGazePoints)GetProcAddress(tobiiLib, "GetNewGazePoints");
    api->GetNewHeadPoses = (tobii_pFnGetNewHeadPoses)GetProcAddress(tobiiLib, "GetNewHeadPoses");
    api->IsInitialised = (tobii_pFnIsInitialised)GetProcAddress(tobiiLib, "IsInitialised");
    api->IsReady = (tobii_pFnIsReady)GetProcAddress(tobiiLib, "IsReady");
    api->IsConnected = (tobii_pFnIsConnected)GetProcAddress(tobiiLib, "IsConnected");
    api->GetUserPresence = (tobii_pFnGetUserPresence)GetProcAddress(tobiiLib, "GetUserPresence");
    api->WasUpdated = (tobii_pFnWasUpdated)GetProcAddress(tobiiLib, "WasUpdated");
    api->TimeSinceLastGazePacket =
        (tobii_pFnTimeSinceLastGazePacket)GetProcAddress(tobiiLib, "TimeSinceLastGazePacket");
    api->TimeSinceLastHeadPacket =
        (tobii_pFnTimeSinceLastHeadPacket)GetProcAddress(tobiiLib, "TimeSinceLastHeadPacket");
    api->GetScreenSizeMm = (tobii_pFnGetScreenSizeMm)GetProcAddress(tobiiLib, "GetScreenSizeMm");

    return true;
}
