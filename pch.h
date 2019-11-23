#pragma once

#include <string>
#include <iostream>
#include <cstdint>
#include <vector>
#include <Windows.h>
#include <chrono>
#include <sstream>

#include "api/main.h"
#include "api/natives.h"
#include "alt-log.h"
#include "Utility.h"
#include "LongerDays.h"

#ifdef _DEBUG
#include <timeapi.h>
#pragma comment(lib, "Winmm.lib")
#endif

#define VERSION "v1.3.2"

using namespace std::chrono_literals;