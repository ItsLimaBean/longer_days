#pragma once

#include <string>
#include <iostream>
#include <cstdint>
#include <vector>
#include <Windows.h>
#include <sstream>
#include <mutex>

#include "api/main.h"
#include "api/natives.h"
#include "alt-log.h"
#include "Utility.h"
#include "singleton.h"

#include "config.h"
#include "memory.h"
#include "script.h"
#include "Pattern.h"
#include "Minhook.h"

#include <timeapi.h>
#pragma comment(lib, "Winmm.lib")

#define VERSION "v3.0.5"
