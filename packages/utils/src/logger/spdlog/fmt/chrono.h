//
// Copyright(c) 2016 Gabi Melman.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)
//

#ifndef UTILS_LOGGER_SPDLOG_FMT_CHRONO_H_
#define UTILS_LOGGER_SPDLOG_FMT_CHRONO_H_
//
// include bundled or external copy of fmtlib's chrono support
//

// #if !defined(SPDLOG_FMT_EXTERNAL)
// #ifdef SPDLOG_HEADER_ONLY
// #ifndef FMT_HEADER_ONLY
#define FMT_HEADER_ONLY
// #endif
// #endif
#include "./bundled/chrono.h"
// #else
// #include <fmt/chrono.h>
// #endif

#endif  // UTILS_LOGGER_SPDLOG_FMT_CHRONO_H_
