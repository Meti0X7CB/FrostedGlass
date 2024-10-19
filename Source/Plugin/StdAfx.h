
#pragma once
#pragma comment(lib, "dwmapi.lib")

// WinAPI
#include <Windows.h>

#include <dwmapi.h>
#include <VersionHelpers.h>

// STL
#include <stdexcept>
#include <string>

// Rainmeter API
#include "../API/RainmeterAPI.h"

#define UNUSED(expr)  \
    do {              \
        (void)(expr); \
    } while (0)
