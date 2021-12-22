// ---------------------------------------------------------------------
// CFXS TImecodeCameraController <https://github.com/CFXS/TimecodeCameraController>
// Copyright (C) 2021 | CFXS
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>
// ---------------------------------------------------------------------
// [CFXS] //
#pragma once

#if defined(CFXS_PLATFORM_WINDOWS)
#define CFXS_DEBUGBREAK() __debugbreak()
#elif defined(CFXS_PLATFORM_LINUX)
#include <signal.h>
#define CFXS_DEBUGBREAK() raise(SIGTRAP)
#else
#error "Platform not defined"
#endif

#define __unused __attribute__((unused))

#define __CLASS__ typeid(*this).name()