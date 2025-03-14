#pragma once

#pragma comment(lib, "Winmm.lib")
#pragma comment(lib, "xinput.lib")
#pragma comment(lib, "shlwapi.lib")

#pragma warning (disable : 4200)

// Made this for myself, disables string encryption so I can upload the .dll on UnknownCheats
// I don't really use the xorstr_ anymore so it stays always defined. Just use VMP
#define BUILD_FOR_UC

// Fix the 'Unicode support requires compiling with /utf-8'
#define FMT_UNICODE 0

#include <SDKDDKVer.h>
#include <Windows.h>
#include <TlHelp32.h>
#include <d3d11.h>
#include <DirectXTK/SimpleMath.h>
#include <Xinput.h>
#include <wrl/client.h>

#include <cinttypes>
#include <cstddef>
#include <cstdint>
#include <cstring>

#include <chrono>
#include <ctime>

#include <filesystem>
#include <fstream>
#include <iostream>
#include <iomanip>

#include <atomic>
#include <mutex>
#include <thread>

#include <memory>
#include <new>

#include <sstream>
#include <string>
#include <string_view>

#include <algorithm>
#include <functional>
#include <utility>

#include <stack>
#include <vector>

#include <typeinfo>
#include <type_traits>

#include <exception>
#include <stdexcept>

#include <any>
#include <optional>
#include <variant>

#include <regex>
#include <random>

#include <fmt/format.h>
#include <nlohmann/json.hpp>

#include "Utilities/xorstr.h"

#include "logger.h"