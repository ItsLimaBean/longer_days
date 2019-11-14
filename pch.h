#pragma once

#include "framework.h"
#include <string>
#include <iostream>
#include <cstdint>
#include <vector>
#include "core/NativeEngine.h"
#include "core/NativeNames.h"
#include "alt-log.h"
#include <timeapi.h>
#include "Utility.h"
#include <sstream>
#include "LongerDays.h"
#include "pattern.h"

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#endif
#pragma comment(lib, "Winmm.lib")

#define DLL_EXPORT __declspec(dllexport)

#define VERSION "v1.2.1"
