﻿#pragma once

#include <string>
#include <iostream>
#include <cstdint>
#include <vector>
#include <Windows.h>
#include <chrono>
#include <sstream>

#include "api/NativeEngine.h"
#include "api/natives.h"
#include "alt-log.h"
#include "Utility.h"
#include "LongerDays.h"

#ifdef _DEBUG
#include <timeapi.h>
#pragma comment(lib, "Winmm.lib")
#endif

#define DLL_EXPORT __declspec(dllexport)
#define VERSION "v1.2.1"

using namespace std::chrono_literals;